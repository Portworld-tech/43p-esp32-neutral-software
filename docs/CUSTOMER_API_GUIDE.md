# Customer SDK API 手册（入口）

> **二次开发请从分章手册开始**（含 Modbus+RS485+LVGL 实战、多技术融合方向、**AI 协作建议**）。

| 语言 | 入口 |
|------|------|
| **中文** | [api_guide/zh/README.md](./api_guide/zh/README.md) |
| **English** | [api_guide/en/README.md](./api_guide/en/README.md) |

## 最快上手

| 目标 | 打开 |
|------|------|
| 人读：怎么二次开发 | [00_secondary_dev.md](./api_guide/zh/00_secondary_dev.md) |
| **售前 / 硬件 FAQ** | [中文 CN](./CUSTOMER_FAQ_CN.md) · [English](./CUSTOMER_FAQ_EN.md) · [排版版](./CUSTOMER_FAQ.md) |
| **AI 辅助开发** | [AI_DEV.md](./api_guide/zh/AI_DEV.md) + 置顶 [AI_CONTEXT.md](./api_guide/zh/AI_CONTEXT.md) |
| 屏控 Modbus 设备 | [G01](./api_guide/zh/guides/G01_modbus_rs485_lvgl.md) + `main/app/examples/` |
| 选产品方向 | [G00](./api_guide/zh/guides/G00_directions.md) |

## 开放层 API

```c
#include "app_api.h"   /* beep / coexist / rs485 / modbus / wifi_util / gpio_out */
```

## 给 AI 的一句话

> 请遵守 `docs/api_guide/zh/AI_CONTEXT.md`：只改开放层；总线勿在 LVGL 回调阻塞；设备态走 `hub_model`。
