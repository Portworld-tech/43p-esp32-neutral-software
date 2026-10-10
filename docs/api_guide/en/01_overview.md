# 1. Overview & layers

> **This guide covers secondary development of the Customer SDK.** For “LVGL + Modbus + RS485 device control”, start with:  
> [0. Getting started](./00_secondary_dev.md) → [G01 walkthrough](./guides/G01_modbus_rs485_lvgl.md)

## Layers

| Layer | Path | Notes |
|-------|------|-------|
| Open application | `main/`, `ui/themes/`, `spiffs_image/` | Editable source |
| Open peripheral APIs | `main/app/app_*.h` | RS485, Modbus, beep, coexist, … |
| Examples | `main/app/examples/` | Bus-bridge skeleton (add to build yourself) |
| Closed libraries | `components/*/include` + `.a` | Wi-Fi / BT / Hub / board |

```c
#include "app_api.h"
```

## Doc map

| Need | Entry |
|------|-------|
| How to develop secondarily | [00](./00_secondary_dev.md) |
| Product directions (7 fusions) | [G00](./guides/G00_directions.md) |
| Technical API chapters | 02–20 in this folder |

---

[← README](./README.md) | [Environment & build →](./02_build.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
