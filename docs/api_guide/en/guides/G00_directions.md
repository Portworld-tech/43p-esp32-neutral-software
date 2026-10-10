# G00. Secondary-development directions (multi-tech fusion)

> This chapter lists **shippable product shapes**. Each direction combines technologies already in this SDK. Pick one, then follow the matching guide.

---

## Overview

| Direction | Technologies | Difficulty | Guide |
|-----------|--------------|------------|-------|
| ① On-panel Modbus controller | LVGL + Hub + RS485 + Modbus + beep | ★★☆ | [G01](./G01_modbus_rs485_lvgl.md) |
| ② Cloud smart panel | LVGL + Wi-Fi + MQTT + Hub + ambient backlight | ★★☆ | [G02](./G02_wifi_mqtt_panel.md) |
| ③ BLE / Mesh local control | LVGL + BLE/Mesh + Hub + coexist | ★★★ | [G03](./G03_ble_local.md) |
| ④ Wired gateway | Ethernet + RS485/Modbus + Hub ops page | ★★★ | [G04](./G04_eth_rs485_gateway.md) |
| ⑤ Whole-home scene hub | ①+②+③, one `hub_model` | ★★★★ | [G05](./G05_full_fusion.md) |
| ⑥ Low-power info display | Ambient + backlight + Wi-Fi/SNTP + sensors | ★★☆ | See “Direction 6” below |
| ⑦ OEM reskin | Themes + SPIFFS icons | ★☆☆ | [Hub](../15_hub.md), [Icons](../16_icons.md) |

Using AI? Pin [AI_CONTEXT](../AI_CONTEXT.md) and read [AI_DEV](../AI_DEV.md).

---

## Direction 1 — On-panel Modbus controller (best fit for “RS485 + LVGL”)

**Story:** Tap “Living room light” on the screen; Modbus over RS485 writes a slave register; the light turns on and the UI stays in sync.

```text
Button → hub_model_toggle → queue → bus worker
       → app_modbus_write_single → slave
       → on success: hub_ui_refresh + app_beep_pulse
```

**You will use:** `hub_ui` / `hub_model`, `gui_task`, `app_rs485`, `app_modbus_rtu`, `app_beep`, `app_coexist` (ETH SPI paused during bus writes).

→ Details: [G01](./G01_modbus_rs485_lvgl.md)

---

## Direction 2 — Cloud smart panel

**Story:** Phone (Bemfa) and the touch panel share the same `hub_model`; idle timeout dims the backlight.

```text
MQTT command → (library) → hub_model
Touch → hub_model → wifi_bemfa_client_schedule_sync()
```

**You will use:** `wifi_management`, `wifi_bemfa_client`, `hub_model`, `UI_AMBIENT_*`.

Can stack with direction 1: cloud commands also go through the same `app_bus_bridge`.

→ [G02](./G02_wifi_mqtt_panel.md)

---

## Direction 3 — Near-field BLE / Mesh

**Story:** Mini-program or provisioner sends SET_STATE; the panel updates; optional Modbus write-out.

```text
BLE SET_STATE → bt_management_apply_set_state (you override)
              → hub_model + optional Modbus
```

Before scans call `app_coexist_before_ble_scan()` so CH390 SPI does not fight the radio.

→ [G03](./G03_ble_local.md)

---

## Direction 4 — Ethernet + RS485 gateway

**Story:** Ethernet uplink (TCP/HTTP you add); Modbus RTU downlink; ops page shows link health.

```text
ETH got IP → custom TCP service → parse → app_modbus_*
hub_model()->protos[] updated for ETH / RS485 health
```

→ [G04](./G04_eth_rs485_gateway.md)

---

## Direction 5 — Whole-home scene hub

**Story:** One “Home” scene: UI updates, MQTT sync, BLE notify neighbors, batch Modbus writes.

Shared rules:

1. Scenes only call `hub_model_apply_scene`  
2. One bridge task owns all bus writes  
3. No channel paints LVGL directly—only the model  

→ [G05](./G05_full_fusion.md)

---

## Direction 6 — Low-power info / read-only meter

**Story:** Hallway panel shows time and temperature; after idle it dims; occasional Wi-Fi sync.

**Tech:** `app_low_power_init`, `UI_AMBIENT_*`, `board_backlight_set`, AHT20 (`ui_bg_task`), optional Ethernet receive-only.

RS485 can stay off; orthogonal to direction 1.

---

## Direction 7 — OEM theme reskin

Edit only `ui/themes/` + `spiffs_image/icons`; business still uses `hub_model`. Fast for branding.

---

## How to choose

| Your goal | Pick |
|-----------|------|
| “No cloud yet—just drive RS485 lights” | Direction 1 |
| “Need a phone app” | Direction 2 (+ optional 1) |
| “Field near-field debug” | Direction 3 |
| “Machine room has Ethernet” | Direction 4 |
| “Full central panel product” | Direction 5 |
| “Mostly display, little interaction” | Direction 6 |
| “New UI skin only” | Direction 7 |

---

## Shared architecture

```text
Open (editable):
  ui/themes/*          look & feel
  main/hub_ui/*        shell (change carefully)
  main/app/app_*.c     bus / beep / coexist
  main/main.c          boot hooks

Closed (headers only):
  hub_model / wifi_* / bt_* / board_*
```

Next: for physical devices open [G01 Modbus + RS485 + LVGL](./G01_modbus_rs485_lvgl.md).  
For AI-assisted edits open [AI_DEV](../AI_DEV.md) and pin [AI_CONTEXT](../AI_CONTEXT.md).
