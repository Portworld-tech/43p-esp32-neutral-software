# AI_CONTEXT — Customer SDK constraint card (for AI)

> Pin this file at the top of the conversation. When it conflicts with generic ESP-IDF advice, **prefer this file and the English API guide in this folder**.

## Project

- Name: ESP32-S3 Customer SDK (smart panel / Hub UI)
- Target: `esp32s3` · ESP-IDF 5.5.x
- Project root: `customer_sdk/` (do not randomly edit a parent private monorepo)

## Architecture (mandatory)

```text
UI / MQTT / BLE ──► hub_model ──► hub_ui_refresh / gui_task_post_lvgl
                      │
                      └─ (optional) queue ──► FreeRTOS task ──► app_rs485 / app_modbus_*
```

- Single source of device truth: `hub_model_*` (`components/hub_core/include/hub_model.h`)
- Open peripheral entry: `#include "app_api.h"` (`main/app/`)
- Cross-thread LVGL updates: must use `gui_task_post_lvgl` (never call `lv_*` from other tasks)

## Allowed edits

- `main/**` (including `main/app/`, `main/hub_ui/`, `main/main.c`)
- `ui/themes/**`
- `spiffs_image/**`
- `docs/**`

## Forbidden

- Editing or reverse-engineering `components/*/lib/**/*.a`
- Treating `esp32_s3_frame` `ui_testbench_*` as product APIs
- Long blocking calls in LVGL event callbacks: `app_rs485_read` / `transact` / `app_modbus_*` / network I/O
- Bypassing `hub_model` and only changing widget text for “device state”
- Calling `esp_wifi_init` again or reinventing RGB display bring-up
- Inventing unpublished GPIO macros or a full expander pin map

## Preferred APIs

| Need | API |
|------|-----|
| RS485 | `app_rs485_init/write/read/transact` |
| Modbus | `app_modbus_read_holding` / `write_single` |
| Beep | `app_beep_pulse` / `click_if_enabled` |
| Coexistence | `app_coexist_before/after_ble_scan` |
| UI state | `hub_model_*` + `hub_ui_refresh` |
| Bus example | `main/app/examples/app_bus_bridge_example.c` |

## RS485 defaults (configurable)

- UART0 · TX43 · RX44 · 115200 · DE = `app_gpio_out_set`
- If the console still owns UART0: switch to USB-JTAG or pass a custom `app_rs485_config_t`

## Doc index (read before implementing)

| Task | Doc |
|------|-----|
| Overview | `docs/api_guide/en/00_secondary_dev.md` |
| AI workflow | `docs/api_guide/en/AI_DEV.md` |
| Modbus + LVGL | `docs/api_guide/en/guides/G01_modbus_rs485_lvgl.md` |
| Directions | `docs/api_guide/en/guides/G00_directions.md` |
| Headers | `docs/api_guide/en/A_headers.md` |

## Output rules (for the AI)

1. First list “files I will modify” and “paths I will not touch”  
2. Wait for confirmation before large code dumps (unless the user asks to edit immediately)  
3. Code must separate queue/worker I/O, or explicitly reuse the example bridge  
4. Self-check against the red-line table in `AI_DEV.md`  

## Language

Answer in the user’s language; code comments may be English or Chinese.
