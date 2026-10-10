# 10. GPIO_OUT / RS485（详细）

> 半双工 RS485 是本工程二次开发控现场设备的基础传输层。Modbus、自定义协议都建立在它之上。

---

## 1. 作用

| API 组 | 作用 |
|--------|------|
| `app_gpio_out_*` | 扩展器 GPIO_OUT；板上用作 RS485 DE/RE |
| `app_rs485_*` | UART 半双工：DE 高→发→等发完→DE 低→收 |

头文件：

```c
#include "app_rs485.h"
#include "app_gpio_out.h"
#include "board_io_expander.h"   /* 安全子集 */
```

---

## 2. 硬件约定（默认）

| 项 | 默认值 | 说明 |
|----|--------|------|
| UART | `UART_NUM_0` | 与 USB-JTAG 控制台搭配使用 |
| TX / RX | GPIO 43 / 44 | WithTheWind 验证值 |
| DE | `app_gpio_out_set(true)` 发送 | 高=发送，低=接收 |
| 波特率 | 115200 8N1 | 可用 config 修改 |

```c
app_rs485_config_t cfg;
app_rs485_get_default_config(&cfg);
cfg.uart_num = UART_NUM_1;   /* 示例：避开控制台 */
cfg.tx_gpio = /* 向硬件方确认 */;
cfg.rx_gpio = /* ... */;
cfg.baud = 9600;
app_rs485_init(&cfg);
```

---

## 3. API 详解

| 函数 | 说明 |
|------|------|
| `app_rs485_init(cfg)` | 安装驱动；`cfg==NULL` 用默认；可重复调用 |
| `app_rs485_is_ready()` | 是否已 init |
| `app_rs485_write(data,len,timeout)` | 半双工写；内部 pause ETH SPI |
| `app_rs485_read(buf,cap,wait_ms)` | 限时读，返回字节数或负错误码 |
| `app_rs485_transact(...)` | 写后读应答 |
| `app_rs485_update_hub_health(ok,health,refresh)` | 更新运维页协议槽 |
| `app_rs485_deinit()` | 释放 UART |

DE 时序（内部）：

```text
DE=1 → delay de_setup_ms → uart_write → wait_tx_done → delay turnaround → DE=0
```

---

## 4. 线程模型（必读）

| 线程 | 允许 |
|------|------|
| LVGL / 按钮回调 | 只 `xQueueSend` 意图，**禁止**长时间 `read/transact` |
| 自研总线任务 | `write` / `read` / `modbus_*` |
| 回写 UI | `gui_task_post_lvgl` 或 `update_hub_health(..., true)` |

完整控灯流程 → [G01](./guides/G01_modbus_rs485_lvgl.md)

---

## 5. Kconfig

| 项 | 默认 | 含义 |
|----|------|------|
| `APP_ENABLE_RS485` | y | 编译本模块 |
| `APP_RS485_AUTO_INIT` | n | `app_main` 自动 `init`（确认无 UART 冲突后再开） |

---

## 6. 常见问题

| 现象 | 排查 |
|------|------|
| 发出去无回复 | DE 极性、A/B、终端电阻、从站地址 |
| 一 init 日志乱码/死机 | UART0 与控制台冲突 → 改 JTAG 或换 UART |
| 触摸卡顿 | 总线调用跑在 LVGL 线程了 |

下一章：[Modbus RTU](./11_modbus.md)
