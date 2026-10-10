# 0. 二次开发怎么开始？

> 本文回答：**拿到 Customer SDK 后，想用 LVGL 屏控 + RS485/Modbus 控设备，该从哪改、怎么串起来？**

---

## 1. 先建立正确心智模型

本工程是「**屏上状态机 + 多种控制通道**」：

```text
                    ┌─────────────┐
  触摸 / 主题 UI ──►│  hub_model  │◄── MQTT / BLE / 自研任务
                    └──────┬──────┘
                           │ hub_ui_refresh()
                           ▼
                      LVGL 界面
                           │
              （可选）业务桥接任务
                           ▼
              app_rs485 / app_modbus_* ──► 现场设备
```

| 你要改的东西 | 改哪里 | 不要改哪里 |
|--------------|--------|------------|
| 界面配色/布局 | `ui/themes/<主题>/` | 封闭 `.a` |
| 房间开关语义 | `hub_model_*` API | 私自另造一套状态 |
| 总线读写 | `main/app/app_rs485*` / `app_modbus*` | 在 LVGL 回调里阻塞读串口 |
| 开机流程 | `main/main.c` | 打乱 NVS→显示→网络 顺序 |

统一头文件：

```c
#include "app_api.h"     /* RS485 / Modbus / 蜂鸣 / 共存 */
#include "hub_model.h"
#include "hub_ui.h"
#include "gui_task.h"
```

---

## 2. 推荐学习路径（按天）

| 阶段 | 目标 | 文档 |
|------|------|------|
| Day 0 | 编译烧录，确认图标与触摸 | [环境与编译](./02_build.md) |
| Day 1 | 只改主题配色 / 一个按钮 toast | [Hub 模型与 UI](./15_hub.md) |
| Day 2 | 理解跨线程：任务里改 label | [跨线程 GUI](./14_gui_task.md) |
| Day 3 | 跑通 RS485 收发 | [GPIO_OUT / RS485](./10_gpio_rs485.md) |
| Day 4 | Modbus 读保持寄存器 | [Modbus RTU](./11_modbus.md) |
| Day 5 | **LVGL 按钮 → Modbus 写寄存器** | [场景：Modbus+RS485+LVGL](./guides/G01_modbus_rs485_lvgl.md) |
| Day 6+ | 选一个融合方向加深 | [开发方向总览](./guides/G00_directions.md) |

---

## 3. 三种合法的二次开发方式

### 方式 A — 只改 UI / 主题（最快）

1. 改 `main/app_ui_theme_select.h` 选主题  
2. 改 `ui/themes/<id>/palette.c`、`theme_local.c`、`pages_*.c`  
3. 按钮回调里只调 `hub_model_*` + `hub_ui_refresh()`  

适合：换皮、加文案、调布局。

### 方式 B — UI + 总线桥接（最常见：Modbus 控灯/窗帘）

1. `app_rs485_init`（注意 UART0 与控制台冲突）  
2. 新建 `main/app/app_bus_bridge.c`：队列收「UI 意图」，任务里调 `app_modbus_write_single`  
3. 主题回调：`hub_model_set_widget_level` → 投递队列 →（可选）MQTT sync  

完整步骤见 [G01](./guides/G01_modbus_rs485_lvgl.md)，示例骨架见 `main/app/examples/`。

### 方式 C — 多通道融合（产品级）

同一 `hub_model` 被以下入口共同改写：

- 触摸 UI  
- 巴法 MQTT  
- BLE `apply_set_state`  
- RS485/Modbus 轮询回读  

原则：**任何入口改完设备态，都更新 hub_model 并刷新 UI**。见 [控制通路](./19_control_map.md) 与 [G00 开发方向](./guides/G00_directions.md)。

---

## 4. 绝对不要做的事

1. 在 LVGL 事件回调里 `while` 等 Modbus 应答（卡死触摸）  
2. 绕过 `hub_model` 只改 LVGL label（云端/蓝牙状态会对不上）  
3. 重复 `esp_wifi_init` / 自己再起一套显示驱动  
4. 依赖测试台 `ui_testbench_*`（那是验证工程，不是产品 API）

---

## 5. 下一步

- 想用 **AI（Cursor/ChatGPT）辅助开发**？→ [AI 协作指南](./AI_DEV.md)（先置顶 [AI_CONTEXT](./AI_CONTEXT.md)）  
- 只想控 RS485 设备？→ [G01 Modbus+RS485+LVGL](./guides/G01_modbus_rs485_lvgl.md)  
- 想做云中控？→ [G02 Wi-Fi+MQTT+Hub](./guides/G02_wifi_mqtt_panel.md)  
- 想看还有哪些产品形态？→ [G00 开发方向总览](./guides/G00_directions.md)
