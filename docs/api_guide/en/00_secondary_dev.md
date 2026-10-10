# 0. How do I start secondary development?

> This page answers: **I have Customer SDK and want LVGL + RS485/Modbus device control — what do I change, and how do the pieces connect?**

---

## 1. Mental model

The firmware is a **on-screen state machine plus several control channels**:

```text
                    ┌─────────────┐
  Touch / theme UI ►│  hub_model  │◄── MQTT / BLE / custom tasks
                    └──────┬──────┘
                           │ hub_ui_refresh()
                           ▼
                      LVGL UI
                           │
              (optional) bus bridge task
                           ▼
              app_rs485 / app_modbus_* ──► field devices
```

| You want to change | Edit here | Do not |
|--------------------|-----------|--------|
| Colors / layout | `ui/themes/<theme>/` | Closed `.a` libraries |
| On/off semantics | `hub_model_*` APIs | Invent a parallel state store |
| Bus I/O | `main/app/app_rs485*` / `app_modbus*` | Block UART reads in LVGL callbacks |
| Boot order | `main/main.c` | Break NVS → display → network order |

Common includes:

```c
#include "app_api.h"     /* RS485 / Modbus / beep / coexist */
#include "hub_model.h"
#include "hub_ui.h"
#include "gui_task.h"
```

---

## 2. Suggested learning path (by day)

| Stage | Goal | Doc |
|-------|------|-----|
| Day 0 | Build, flash, icons + touch OK | [Build](./02_build.md) |
| Day 1 | Theme colors / one toast button | [Hub model & UI](./15_hub.md) |
| Day 2 | Cross-thread label updates | [GUI task](./14_gui_task.md) |
| Day 3 | RS485 TX/RX works | [GPIO_OUT / RS485](./10_gpio_rs485.md) |
| Day 4 | Modbus read holding registers | [Modbus RTU](./11_modbus.md) |
| Day 5 | **LVGL button → Modbus write** | [G01 guide](./guides/G01_modbus_rs485_lvgl.md) |
| Day 6+ | Pick a fusion direction | [G00 directions](./guides/G00_directions.md) |

Using AI tools? → [AI-assisted development](./AI_DEV.md) (pin [AI_CONTEXT](./AI_CONTEXT.md) first).

---

## 3. Three valid approaches

### A — UI / theme only (fastest)

1. Set `APP_UI_THEME_ID` in `main/app_ui_theme_select.h`  
2. Edit `ui/themes/<id>/palette.c`, `theme_local.c`, `pages_*.c`  
3. In callbacks call only `hub_model_*` + `hub_ui_refresh()`  

Best for reskins and copy changes.

### B — UI + bus bridge (typical: Modbus lights / curtains)

1. `app_rs485_init` (watch UART0 vs console conflict)  
2. Add `main/app/app_bus_bridge.c`: queue of “UI intents”, worker calls `app_modbus_write_single`  
3. Theme callback: `hub_model_set_widget_level` → enqueue → optional MQTT sync  

Full steps: [G01](./guides/G01_modbus_rs485_lvgl.md). Skeleton: `main/app/examples/`.

### C — Multi-channel product

The same `hub_model` is updated from:

- Touch UI  
- Bemfa MQTT  
- BLE `apply_set_state`  
- RS485/Modbus poll-back  

Rule: **after any path changes device state, update `hub_model` and refresh the UI**. See [Control map](./19_control_map.md) and [G00](./guides/G00_directions.md).

---

## 4. Never do this

1. `while`-wait for Modbus replies inside LVGL event callbacks  
2. Skip `hub_model` and only paint labels (cloud/BLE will desync)  
3. Call `esp_wifi_init` again / reinvent the display stack  
4. Depend on testbench `ui_testbench_*` (validation project, not product APIs)

---

## 5. Next steps

- **AI-assisted coding** → [AI_DEV.md](./AI_DEV.md)  
- RS485 devices only → [G01](./guides/G01_modbus_rs485_lvgl.md)  
- Cloud panel → [G02](./guides/G02_wifi_mqtt_panel.md)  
- More product shapes → [G00](./guides/G00_directions.md)
