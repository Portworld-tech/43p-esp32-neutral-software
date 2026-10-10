# AI_CONTEXT — Customer SDK 约束卡（给 AI 阅读）

> 将本文件置于对话上下文顶部。与常识冲突时，**以本文件与同目录 API 手册为准**。

## 项目

- 名称：ESP32-S3 Customer SDK（智能中控 / Hub UI）
- 芯片：`esp32s3` · ESP-IDF 5.5.x
- 工程根：`customer_sdk/`（不要到上级私有仓乱改）

## 架构（必须遵守）

```text
UI / MQTT / BLE ──► hub_model ──► hub_ui_refresh / gui_task_post_lvgl
                      │
                      └─（可选）队列 ──► FreeRTOS 任务 ──► app_rs485 / app_modbus_*
```

- 设备态唯一真相：`hub_model_*`（`components/hub_core/include/hub_model.h`）
- 开放外设入口：`#include "app_api.h"`（`main/app/`）
- 跨线程改 LVGL：必须 `gui_task_post_lvgl`（禁止在其它任务直接 `lv_*`）

## 允许修改

- `main/**`（含 `main/app/`、`main/hub_ui/`、`main/main.c`）
- `ui/themes/**`
- `spiffs_image/**`
- `docs/**`

## 禁止

- 修改或反编译 `components/*/lib/**/*.a`
- 使用 `esp32_s3_frame` 的 `ui_testbench_*` 作为产品 API
- 在 LVGL 事件回调中长时间阻塞：`app_rs485_read` / `transact` / `app_modbus_*` / 网络
- 绕过 `hub_model` 只改控件文字冒充设备态
- 重复 `esp_wifi_init` / 自建一套 RGB 显示初始化
- 臆造未公开的 GPIO 宏或 expander 完整引脚表

## 推荐 API

| 需求 | API |
|------|-----|
| RS485 | `app_rs485_init/write/read/transact` |
| Modbus | `app_modbus_read_holding` / `write_single` |
| 蜂鸣 | `app_beep_pulse` / `click_if_enabled` |
| 共存 | `app_coexist_before/after_ble_scan` |
| UI 状态 | `hub_model_*` + `hub_ui_refresh` |
| 总线示例 | `main/app/examples/app_bus_bridge_example.c` |

## RS485 默认（可配置）

- UART0 · TX43 · RX44 · 115200 · DE=`app_gpio_out_set`
- 若控制台占用 UART0：改 USB-JTAG 或换 `app_rs485_config_t`

## 文档索引（实现前应阅读）

| 任务 | 文档 |
|------|------|
| 总则 | `docs/api_guide/zh/00_secondary_dev.md` |
| AI 协作 | `docs/api_guide/zh/AI_DEV.md` |
| Modbus+LVGL | `docs/api_guide/zh/guides/G01_modbus_rs485_lvgl.md` |
| 方向选型 | `docs/api_guide/zh/guides/G00_directions.md` |
| 头文件 | `docs/api_guide/zh/A_headers.md` |

## 输出要求（对 AI）

1. 先列「将修改的文件」与「不会修改的路径」  
2. 计划需我确认后再写大段代码（除非用户要求直接改）  
3. 代码须含：队列/任务分离 或明确引用 example 桥接  
4. 自检：对照 `AI_DEV.md` 红线清单  

## 语言

默认用客户提问的语言回答；代码注释可用中文或英文。
