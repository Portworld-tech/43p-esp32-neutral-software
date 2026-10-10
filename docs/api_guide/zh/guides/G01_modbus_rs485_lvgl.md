# G01. 场景详解：Modbus + RS485 + LVGL 控设备

> **目标：** 在本工程上，用触摸 UI 控制 RS485 总线上的 Modbus 从站（灯 / 窗帘 / 继电器等）。  
> **读者：** 已能 `idf.py flash`，不清楚「按钮点下去之后代码该往哪写」。

---

## 1. 数据流（先看懂再写代码）

```text
┌──────────────┐     ① 改模型      ┌────────────┐
│ LVGL 按钮回调 │ ───────────────► │ hub_model  │
└──────┬───────┘                   └─────┬──────┘
       │ ② 投递意图（非阻塞）              │ ③ hub_ui_refresh()
       ▼                                 ▼
┌──────────────┐                   ┌────────────┐
│ FreeRTOS 队列 │                   │  屏幕刷新   │
└──────┬───────┘                   └────────────┘
       │ ④ 总线任务取出
       ▼
┌──────────────┐  ⑤ DE+UART   ┌────────────┐
│ app_modbus_* │ ───────────► │ RS485 从站  │
└──────┬───────┘              └────────────┘
       │ ⑥ 成功/失败
       ▼
  app_beep / update_hub_health /（可选）MQTT sync
```

**关键：①② 在 LVGL 线程只做「改模型 + 入队」；⑤ 必须在独立任务。**

---

## 2. 硬件与 Kconfig 准备

### 2.1 默认引脚（WithTheWind 验证值）

| 信号 | 默认 |
|------|------|
| UART | UART0 |
| TX / RX | GPIO43 / GPIO44 |
| DE/RE | 扩展器 GPIO_OUT（高=发送） |
| 波特率 | 115200 8N1 |

量产前请向硬件供应商确认；可用 `app_rs485_config_t` 改口。

### 2.2 控制台冲突

若 `sdkconfig` 仍是 `CONFIG_ESP_CONSOLE_UART_NUM=0`，与 RS485 争用。二选一：

1. menuconfig → 控制台改为 **USB Serial/JTAG**（推荐，与测试验证一致）  
2. 或 `app_rs485_config_t` 改用 `UART_NUM_1` + 其它 TX/RX

### 2.3 menuconfig

```
Secondary-dev peripherals
  [*] APP_ENABLE_RS485
  [ ] APP_RS485_AUTO_INIT     ← 建议先手动 init，确认无冲突再打开
```

---

## 3. 分步实现

### 步骤 A — 确认总线通（无 UI）

在 `app_main` 显示初始化之后临时加：

```c
#include "app_api.h"

app_rs485_init(NULL);
uint16_t reg = 0;
esp_err_t e = app_modbus_read_holding(0x01, 0x0000, 1, &reg, 1, 800);
ESP_LOGI("demo", "modbus read: %s val=%u", esp_err_to_name(e), reg);
```

串口日志成功后再接 UI。失败排查：DE 极性、A/B、从站地址、波特率、是否与控制台冲突。

### 步骤 B — 建立「意图队列」（推荐文件）

复制示例：

- `main/app/examples/app_bus_bridge_example.c` → `main/app/app_bus_bridge.c`  
- 同目录头文件 `app_bus_bridge.h`  
- 在 `main/CMakeLists.txt` 的 `MAIN_SRCS` 加入该 `.c`

意图结构示例：

```c
typedef enum {
    BUS_CMD_WRITE_HOLDING = 1,
    BUS_CMD_READ_HOLDING,
} bus_cmd_t;

typedef struct {
    bus_cmd_t cmd;
    uint8_t slave;
    uint16_t addr;
    uint16_t value;   /* write */
    int room, slot;   /* 回写 UI 时定位 widget */
} bus_job_t;
```

任务循环：

```c
for (;;) {
    bus_job_t job;
    if (xQueueReceive(q, &job, portMAX_DELAY) != pdTRUE) continue;

    esp_err_t err = ESP_FAIL;
    if (job.cmd == BUS_CMD_WRITE_HOLDING) {
        err = app_modbus_write_single(job.slave, job.addr, job.value, 800);
    }
    /* 成功：蜂鸣；失败：toast + 健康度 */
    app_rs485_update_hub_health(err == ESP_OK, err == ESP_OK ? 95 : 20, true);
    if (err == ESP_OK) {
        app_beep_pulse(60);
    } else {
        hub_model_toast("总线写入失败");
        gui_task_post_lvgl(refresh_cb, NULL);
    }
}
```

### 步骤 C — LVGL / 主题回调只入队

在主题 `theme_local.c` 或房间页按钮回调：

```c
#include "hub_model.h"
#include "hub_ui.h"
#include "app_bus_bridge.h"   /* 你的桥接 */
#include "app_beep.h"

static void on_light_toggle(lv_event_t *e)
{
    (void)e;
    const int room = 0, slot = 0;
    hub_model_toggle_widget(room, slot);
    hub_ui_refresh();
    app_beep_click_if_enabled();

    hub_widget_t *w = hub_model_widget_by_slot(room, slot);
    if (w == NULL) return;

    /* 约定：从站 1，寄存器 0 = 开关 0/1 */
    bus_bridge_post_write(1, 0x0000, w->on ? 1 : 0, room, slot);
}
```

**不要**在这里调用 `app_modbus_write_single`。

### 步骤 D — 回读同步（可选）

定时任务每 2 s：

```c
uint16_t v = 0;
if (app_modbus_read_holding(1, 0, 1, &v, 1, 500) == ESP_OK) {
    hub_model_set_widget_level(0, 0, v ? 100 : 0);
    /* 或按 on/off 语义 set */
    gui_task_post_lvgl(refresh_cb, NULL);
}
```

用于：现场手动扳了开关，屏上要跟上。

### 步骤 E — 与 MQTT 融合（可选）

本地写成功后：

```c
wifi_bemfa_client_publish_status_u8(item_id, value, true);
wifi_bemfa_client_schedule_sync();
```

云端下发时：在你的 `apply_set_state` 或 MQTT 扩展里 **同样** `bus_bridge_post_write`，保证「手机 / 屏 / 总线」一致。

---

## 4. 寄存器映射表示例（请按设备手册改）

| Hub widget | 从站 | 功能码 | 地址 | 值含义 |
|------------|------|--------|------|--------|
| 客厅灯 ONOFF | 1 | 06 | 0x0000 | 0=关 1=开 |
| 调光 DIMMER | 1 | 06 | 0x0001 | 0–100 |
| 窗帘 CURTAIN | 2 | 06 | 0x0010 | 0–100% |

建议在 `app_bus_bridge.c` 里做一张静态表 `widget → (slave,addr)`，避免魔法数散落主题代码。

---

## 5. 验收清单

- [ ] 无 UI 时 `read_holding` 成功  
- [ ] 触摸开关，示波器 / 从站可见写帧  
- [ ] 触摸过程不卡顿（总线在独立任务）  
- [ ] 拔掉总线后 toast / 健康度下降  
- [ ] （可选）手机 MQTT 与屏状态一致  

---

## 6. 相关 API 速查

| 需求 | API |
|------|-----|
| 初始化总线 | `app_rs485_init` |
| 写寄存器 | `app_modbus_write_single` |
| 读寄存器 | `app_modbus_read_holding` |
| 改屏上设备态 | `hub_model_toggle_widget` / `set_widget_level` |
| 刷新界面 | `hub_ui_refresh` 或 `gui_task_post_lvgl` |
| 蜂鸣反馈 | `app_beep_pulse` / `click_if_enabled` |
| 协议槽 | `app_rs485_update_hub_health` |

技术细节：[RS485](../10_gpio_rs485.md) · [Modbus](../11_modbus.md) · [Hub](../15_hub.md) · [gui_task](../14_gui_task.md)

示例源码：`main/app/examples/app_bus_bridge_example.c`
