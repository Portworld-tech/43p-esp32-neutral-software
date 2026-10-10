# 1. 概览与分层

> **本手册描述 Customer SDK 二次开发。** 若你要做「LVGL + Modbus + RS485 控设备」，请先读：  
> [0. 二次开发怎么开始](./00_secondary_dev.md) → [G01 实战](./guides/G01_modbus_rs485_lvgl.md)

## 分层

| 层 | 路径 | 说明 |
|----|------|------|
| 开放业务 | `main/`、`ui/themes/`、`spiffs_image/` | 可改源码 |
| 开放外设 API | `main/app/app_*.h` | RS485、Modbus、蜂鸣、共存等 |
| 示例 | `main/app/examples/` | 总线桥接骨架（需自行加入编译） |
| 封闭库 | `components/*/include` + `.a` | Wi-Fi / BT / Hub / 板级 |

```c
#include "app_api.h"
```

## 文档地图

| 类型 | 入口 |
|------|------|
| 怎么二次开发 | [00](./00_secondary_dev.md) |
| 产品方向（7 种融合） | [G00](./guides/G00_directions.md) |
| 技术 API 分章 | 本目录 02–20 |
