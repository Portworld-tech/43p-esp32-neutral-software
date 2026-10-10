# Customer SDK Secondary Development Overview (Zero to Production Customization)

This guide is for engineers who received `customer_sdk/`: what the project is, what you can change, how to build and flash, Hub themes/icons, the Hub business model, **AHT20 temperature/humidity calibration**, **RS485 / Modbus device control**, plus Wi‑Fi / Bemfa MQTT / Bluetooth extensions.

Treat this directory as a **standalone ESP-IDF project**. Do not mix it with the vendor’s private product repository root.

> This public repository ships the Chinese/English secondary-dev overview. Chaptered `docs/api_guide/` is delivered with the contract SDK and is not in this GitHub tree.

| Document | Purpose |
|----------|---------|
| [THEME_SECONDARY_DEV.md](./THEME_SECONDARY_DEV.md) | Chinese edition（same structure） |
| **This document (English)** | Secondary-development overview (start here) |
| [api_guide/en/00_secondary_dev.md](./api_guide/en/00_secondary_dev.md) | Mental model + three development styles + learning path |
| [api_guide/en/guides/G01_modbus_rs485_lvgl.md](./api_guide/en/guides/G01_modbus_rs485_lvgl.md) | **Panel + RS485/Modbus hands-on** (most common) |
| [api_guide/en/guides/G00_directions.md](./api_guide/en/guides/G00_directions.md) | Seven product fusion directions |
| [api_guide/en/AI_DEV.md](./api_guide/en/AI_DEV.md) | Cursor / ChatGPT collaboration (incl. red lines) |

Chinese API guide: [api_guide/zh/README.md](./api_guide/zh/README.md)

---

## 0. Five-minute summary

| Question | Answer |
|----------|--------|
| What is this? | A **layered customer package** for ESP32-S3 + LVGL smart HMI firmware: board/cloud/BT/business model as prebuilt libraries; UI themes, assets, and open peripheral APIs are editable |
| Target chip | `esp32s3` (prebuilt `.a` for this target only) |
| IDF version | **ESP-IDF 5.5.x** |
| Default UI | One of ten Hub themes (default `SLATE`); **no** SquareLine `default` pack |
| How is T/H shown? | AHT20 → `ui_bg_task` → `hub_model()->indoor_c` / `rh` → theme home (e.g. Slate temp/humidity cards) |
| How to calibrate readings? | Edit open-layer **`main/board/aht20_calib.h`** macros, then rebuild and flash (see §11) |
| How to control RS485 devices? | Open-layer **`app_rs485_*` / `app_modbus_*`** + a bus-bridge task (see §12); **never** block read/write inside LVGL callbacks |
| Flash note | You **must** program **storage (SPIFFS)** or icons stay blank |

Unified open peripheral entry:

```c
#include "app_api.h"   /* RS485 / Modbus / GPIO_OUT / beep / coexist / Wi-Fi helpers */
```

---

## 1. Product and software architecture

### 1.1 Hardware capabilities (board layer encapsulated)

Typical delivery board (Withthewind class):

- MCU: ESP32-S3
- Display: ST7701 RGB **480×480**
- Touch: GT911 (I2C: SCL=IO7, SDA=IO15)
- IO expander: NCA9555 (beep, RS485 DE, etc.)
- Backlight: MCU **GPIO4 LEDC** software PWM (`board_bsp.a`)
- Optional: AHT20 T/H (shared I2C with touch), CH390 SPI Ethernet
- RS485: open layer default **UART0 · TX43 · RX44 · 115200** + expander DE (configurable)

**Full pin maps and expander nets are not published in the customer package**; use public headers:

- `components/board_bsp/include/withthewind_board_lvgl_init.h`
- `components/board_bsp/include/board_io_expander.h` (safe subset)
- `components/board_bsp/include/board_ethernet_ch390.h`
- `components/board_bsp/include/aht20.h`
- `main/app/app_*.h` (RS485 / Modbus / beep / coexist)

### 1.2 Layered model (open / closed)

```
┌──────────────────────────────────────────────────────────────┐
│  Open layer (editable source)                                │
│  ui/themes/* · main/hub_ui · main/app · main/board           │
│  main/gui · spiffs_image · main/main.c                       │
└──────────────────────────▲───────────────────────────────────┘
                           │ call public APIs
┌──────────────────────────┴───────────────────────────────────┐
│  Closed layer (.a + include/*.h only)                        │
│  board_bsp · hub_core · cloud_wifi · bt_ctrl                 │
└──────────────────────────────────────────────────────────────┘
                           │
                    ESP-IDF / LVGL / driver components
```

| Open (editable) | Closed (`.a` — do not edit or reverse) |
|---|---|
| `ui/themes/<theme>/` | `components/board_bsp` (display/touch/backlight/ETH) |
| `main/hub_ui/`, `main/gui/` | `components/hub_core` (`hub_model` implementation) |
| `main/app/` (RS485/Modbus/beep/coexist…) | `components/cloud_wifi` (Wi‑Fi + Bemfa MQTT) |
| `main/board/aht20*.c`, `aht20_calib.h` | `components/bt_ctrl` (BLE / Mesh) |
| `main/app_ui_theme_select.h`, `main/main.c` | |
| `spiffs_image/`, `main/app/examples/` | |

Closed library path: `components/*/lib/esp32s3/*.a`.

### 1.3 Runtime data flow (where control enters / exits)

```
Phone App / Bemfa cloud ──MQTT──► cloud_wifi ──┐
Phone BLE / Mesh        ────────► bt_ctrl     ──┤
Touch UI                ────────► hub_model_* ◄─┘──► hub_ui_refresh / toast
                              │
                    (optional) bus-bridge queue/task
                              ▼
              app_rs485 / app_modbus_* ──► field devices (lights/curtains/relays…)

AHT20 ──ui_bg_task──► hub_model.indoor_c / rh ──► theme home T/H cards
```

**Rule:** `hub_model` is the single source of truth for device state; refresh UI after any channel changes state; bus I/O must run in a dedicated task.

---

## 2. Repository map

```
customer_sdk/
├── README.md / AGENTS.md
├── docs/
│   ├── THEME_SECONDARY_DEV.md     # Chinese edition
│   ├── THEME_SECONDARY_DEV_EN.md  # this document
│   └── api_guide/zh|en/           # ★ chaptered API + scenario guides + AI notes
├── CMakeLists.txt / sdkconfig.defaults / partitions.csv
├── main/
│   ├── main.c                     # boot entry
│   ├── app_ui_theme_select.h      # ★ theme select (one macro)
│   ├── Kconfig.projbuild          # MQTT/BLE/AHT20/RS485, etc.
│   ├── app/                       # ★ open peripheral APIs
│   │   ├── app_api.h              # umbrella header
│   │   └── examples/              # bus-bridge skeleton (not built by default)
│   ├── board/                     # ★ AHT20 driver + aht20_calib.h
│   ├── gui/ui_bg_task.*           # AHT20 background poll → hub_model
│   ├── hub_ui/                    # shared UI shell and icons
│   └── ...
├── ui/themes/slate|sand|…         # ten themes (default slate)
├── spiffs_image/icons/nt/*.png
└── components/
    ├── board_bsp/include/
    ├── hub_core/include/hub_model.h
    ├── cloud_wifi/include/
    └── bt_ctrl/include/
```

Partitions (`partitions.csv`): dual OTA (5M each) + **storage SPIFFS 5M**.

---

## 3. Environment and first build/flash

### 3.1 Prepare

1. Install **ESP-IDF 5.5.x** and export the environment (`export.ps1` / `export.sh`).
2. Open **only** the `customer_sdk/` directory.
3. Note your serial port (e.g. `COM5`).

### 3.2 Commands

```powershell
cd <path-to>/customer_sdk
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

After changing theme: `idf.py reconfigure && idf.py build`.

### 3.3 SPIFFS is required

A normal `idf.py flash` writes `storage`. Flashing app only → “text without icons”.

### 3.4 Common menuconfig items (`main/Kconfig.projbuild`)

| Config | Default tendency | Meaning |
|--------|------------------|---------|
| `APP_FEATURE_MQTT` | y | Bemfa MQTT |
| `APP_FEATURE_BLE` / `MESH` | y | Bluetooth / Mesh |
| `AHT20_ENABLE` | y | Onboard/external AHT20; background write to `hub_model` |
| `APP_ENABLE_RS485` | y | Build RS485/Modbus open APIs |
| `APP_RS485_AUTO_INIT` | **n** | Auto `app_rs485_init` at boot (enable only after UART conflict is cleared) |
| `UI_AMBIENT_*` | — | Standby backlight dimming |

LVGL PNG + POSIX FS are already enabled in `sdkconfig.defaults`; do not turn them off casually.

---

## 4. Ten Hub themes

### 4.1 Capability boundary

- All ten themes **share** `main/hub_ui` page logic and routing.
- Each theme only swaps colors, layout, and home composition.
- SquareLine `default` theme pack is **not** provided.

### 4.2 Catalog

| Macro | Directory | Notes |
|---|---|---|
| `SLATE` | `ui/themes/slate/` | Deep-gray business (default) |
| `SAND` … `METRO` | `ui/themes/<id>/` | Other nine; see `app_ui_theme_select.h` |

Theme directories usually contain: `boot.c`, `palette.c`, `theme_local.*`, `home.c`, `pages_*.c`.

### 4.3 Switching themes

Edit `main/app_ui_theme_select.h`:

```c
#define APP_UI_THEME_ID APP_UI_THEME_SLATE
```

Then `idf.py reconfigure && idf.py build && idf.py -p COMx flash`.

### 4.4 Where do temperature and humidity appear?

Default **Slate** example: middle metrics row is **power / temperature / humidity**, reading:

- `hub_model()->indoor_c`
- `hub_model()->rh`

Implementation: `ui/themes/slate/home.c`.  
Other themes (ocean / forest / ink…) also bind `indoor_c` / `rh` with different copy/layout.

When data is not ready (AHT20 not sampled yet), Slate shows `--.-°C` / `--%` so demo model defaults are not mistaken for live values.

### 4.5 New themes / change strategy

| Goal | Where to edit |
|------|----------------|
| Colors / radius only | Current theme `palette.c`, `theme_local.c` |
| One page layout | `home.c` / `pages_*.c` |
| Navigation / network page logic | `main/hub_ui/` (smoke-test multiple themes) |
| Device point semantics | `hub_model_*` — do not bypass the model |
| Buttons driving RS485 | Callback only updates model + **enqueues**; see §12 |

New theme: copy `ui/themes/_template/` → register in `app_ui_theme_select.h` + `resolve_ui_theme.cmake`.

---

## 5. Icons and SPIFFS

1. Files: `spiffs_image/icons/nt/<name>.png`
2. Runtime path: `S:/icons/nt/<name>.png` (`hub_icons.c`)
3. Requires: `CONFIG_APP_USE_SPIFFS_UI_ASSETS` + `LV_USE_PNG` + `LV_USE_FS_POSIX` (letter=`S`)

Missing PNG/FS/storage flash → blank icons. Check logs: `spiffs mounted`, `hub_ico: PSRAM icon cache xx/35`.

---

## 6. Business model API (`hub_model`)

Header: `components/hub_core/include/hub_model.h`.

```c
#include "hub_model.h"
#include "hub_ui.h"

hub_model_t *m = hub_model();
hub_model_set_room(0);
hub_model_apply_scene("home");
hub_model_toggle_widget(room, slot);
hub_model_set_widget_level(room, slot, 80);
hub_model_toast("Done");
hub_ui_refresh();
```

Cross-thread LVGL updates: **must** use `gui_task_post_lvgl(...)` (see `main/gui/gui_task.h`). Do not call `lv_*` directly from Wi‑Fi/bus tasks.

---

## 7. Wi‑Fi

Header: `wifi_management.h`. Open-layer sorted scan helper: `app_wifi_util.h` (`app_wifi_scan_sorted`).

On-screen network page is already wired; do not start a second `esp_wifi_init` in custom code.

---

## 8. MQTT cloud (Bemfa)

Header: `wifi_bemfa_client.h`. Requires `APP_FEATURE_MQTT=y`.

After local state changes: `hub_model_*` → `hub_ui_refresh()` → `wifi_bemfa_client_schedule_sync()`.  
If you also drive RS485: **cloud and touch must share the same bus bridge** (see §12 / G01) to avoid split state.

---

## 9. Bluetooth (BLE / Mesh)

Headers: `bt_management.h`, `bt_proto.h`.  
Call `app_coexist_before/after_ble_scan()` around scans to reduce conflict with CH390 SPI.  
Points can override `bt_management_apply_set_state` to update `hub_model` and enqueue Modbus writes.

---

## 10. Ethernet (CH390)

Header: `board_ethernet_ch390.h`.  
`app_rs485_write` already pauses ETH SPI via `app_coexist_*_rs485_burst` to reduce drop risk during bus bursts.

---

## 11. AHT20 temperature/humidity: display, calibration, troubleshooting (focus)

### 11.1 Data path (already wired)

```
AHT20 (I2C shared with touch)
    │  ui_bg_task ~10s poll (CONFIG_AHT20_ENABLE=y)
    ▼
aht20_read()  ← applies aht20_calib.h
    ▼
ui_runtime_indoor_apply_climate(temp, rh)
    ▼
hub_model()->indoor_c  /  hub_model()->rh
    ▼
hub_ui_refresh() → theme home temperature/humidity cards
```

Related sources:

| File | Role |
|------|------|
| `main/gui/ui_bg_task.c` | Background init/poll; posts to LVGL on success |
| `main/hub_ui/ui_runtime_stubs.c` | Writes `hub_model` and refresh |
| `main/board/aht20.c` | Driver (open) |
| `main/board/aht20_calib.h` | **★ Customer calibration macros** |
| `ui/themes/slate/home.c` | Home display (default theme example) |

Log keywords: `AHT20 ready (bg task)`; long failures back off (board may ship without AHT20).

### 11.2 Field calibration (most common customer edit)

Edit **`main/board/aht20_calib.h`**. Formula:

```text
temp_out (°C) = temp_raw × AHT20_TEMP_SCALE + AHT20_TEMP_OFFSET_C
rh_out  (%)   = rh_raw   × AHT20_RH_SCALE   + AHT20_RH_OFFSET_PCT
```

| Macro | Default | Usage |
|-------|---------|-------|
| `AHT20_CALIB_ENABLE` | `1` | `0` = disable calibration |
| `AHT20_TEMP_OFFSET_C` | `0.0f` | Negative if reading high, e.g. `-1.5f` |
| `AHT20_TEMP_SCALE` | `1.0f` | Gain trim |
| `AHT20_RH_OFFSET_PCT` | `0.0f` | Humidity offset |
| `AHT20_RH_SCALE` | `1.0f` | Humidity gain |
| `AHT20_RH_CLAMP` | `1` | Clamp RH to 0..100 after calibration |

Example:

```c
/* main/board/aht20_calib.h */
#define AHT20_TEMP_OFFSET_C   (-1.5f)   /* UI reads 1.5°C high */
#define AHT20_RH_OFFSET_PCT   (3.0f)    /* humidity 3% low */
```

Then:

```powershell
idf.py build
idf.py -p COMx flash monitor
```

Tuning tips:

1. Use `aht20_read_raw` against a reference meter  
2. Adjust OFFSET/SCALE  
3. After flash, check home readings (same path as `aht20_read`)  
4. **Do not subtract the offset again in the theme** (calibrate once in the driver)

### 11.3 Manual read API

```c
#include "aht20.h"
#include "withthewind_board_lvgl_init.h"

aht20_init(board_i2c_get_handle());
float t = 0, rh = 0;
aht20_read(&t, &rh);       /* calibrated */
aht20_read_raw(&t, &rh);   /* uncalibrated — for tuning OFFSET */
```

Valid sample available: `ui_bg_task_indoor_ready()` (`main/gui/ui_bg_task.h`).

### 11.4 Troubleshooting

| Symptom | Check |
|---------|-------|
| Home stuck on `--` | Module missing/wrong orientation, `AHT20_ENABLE`, I2C conflict, no `AHT20 ready` log |
| Numbers present but fixed offset vs meter | Adjust `aht20_calib.h` OFFSET |
| Macro change ignored | Rebuild/flash done? Theme editing a different state path? |
| Touch glitches | AHT20 shares bus with GT911; check wiring and address `0x38` |

---

## 12. RS485 / Modbus control (focus)

The open layer already provides half-duplex transport + a thin Modbus client validated on the bring-up bench — **you do not need to rewrite DE timing from scratch**.

### 12.1 API overview

```c
#include "app_api.h"

app_rs485_init(NULL);   /* or pass a custom app_rs485_config_t */
uint16_t reg = 0;
esp_err_t e = app_modbus_read_holding(0x01, 0x0000, 1, &reg, 1, 700);
e = app_modbus_write_single(0x01, 0x0000, 0x0001, 700);
app_rs485_update_hub_health(e == ESP_OK, e == ESP_OK ? 95 : 20, true);
```

| API | Header | Role |
|-----|--------|------|
| `app_rs485_init/write/read/transact` | `app_rs485.h` | Half-duplex UART + DE |
| `app_modbus_read_holding` (FC03) | `app_modbus_rtu.h` | Read holding registers |
| `app_modbus_write_single` (FC06) | `app_modbus_rtu.h` | Write single register |
| `app_gpio_out_set` | `app_gpio_out.h` | DE/RE (high = transmit) |
| `app_coexist_*_rs485_burst` | `app_coexist.h` | Pause ETH during TX burst (already called inside write) |
| `app_beep_pulse` / `click_if_enabled` | `app_beep.h` | Operation feedback sound |

Default hardware: `UART0` · TX **GPIO43** · RX **GPIO44** · 115200 8N1 · DE=`app_gpio_out_set`.

### 12.2 Must-read: console UART conflict

If `sdkconfig` also uses UART0 for the console, it **conflicts** with default RS485. Pick one:

1. menuconfig → console to **USB Serial/JTAG** (recommended; matches validation setup)  
2. Or customize:

```c
app_rs485_config_t cfg;
app_rs485_get_default_config(&cfg);
cfg.uart_num = UART_NUM_1;
cfg.tx_gpio = /* confirm with hardware vendor */;
cfg.rx_gpio = /* ... */;
cfg.baud = 9600;
app_rs485_init(&cfg);
```

`APP_RS485_AUTO_INIT` defaults **off**; enable it or call `init` manually in `main.c` after the conflict is resolved.

### 12.3 Threading model (wrong usage freezes touch)

| Thread | Allowed | Forbidden |
|--------|---------|-----------|
| LVGL / button callbacks | `hub_model_*`, `hub_ui_refresh`, `QueueSend` intents | `app_modbus_*` / long `read` |
| Bus task | `app_rs485_*` / `app_modbus_*` | Direct `lv_*` |
| UI write-back | `gui_task_post_lvgl` or `update_hub_health(..., true)` | — |

### 12.4 Recommended wiring: bus bridge (UI → Modbus)

Full steps: **[G01](./api_guide/en/guides/G01_modbus_rs485_lvgl.md)**. Summary:

1. Prove `app_modbus_read_holding` works without UI  
2. Copy the example:

```
main/app/examples/app_bus_bridge_example.c  →  main/app/app_bus_bridge.c
main/app/examples/app_bus_bridge_example.h  →  main/app/app_bus_bridge.h
```

3. Add the `.c` to `MAIN_SRCS` in `main/CMakeLists.txt`  
4. After `gui_task_init()`, call `app_bus_bridge_start()`  
5. Theme buttons:

```c
hub_model_toggle_widget(room, slot);
hub_ui_refresh();
app_beep_click_if_enabled();
app_bus_bridge_post_write(slave, reg_addr, on ? 1 : 0);  /* non-blocking enqueue */
```

6. (Optional) Cloud commands call the same `post_write` so phone/panel/bus stay consistent  

### 12.5 More function codes

FC05/0F/10: build frames with `app_modbus_crc16` + `app_rs485_transact`; or bring in esp-modbus on the **same** UART (do not mix concurrent use with this thin wrapper).

### 12.6 Troubleshooting

| Symptom | Check |
|---------|-------|
| No response | DE polarity, A/B, termination, slave address, baud |
| Garbled logs / crash after init | UART0 vs console conflict |
| Touch stutter | Bus calls running on LVGL thread |
| ETH drops | Ensure path uses `app_rs485_write` (includes coexist pause) |
| Missing expander symbols at link | Customer `board_io_expander.h` subset + link `board_bsp.a` |

Chapter details: [RS485](./api_guide/en/10_gpio_rs485.md) · [Modbus](./api_guide/en/11_modbus.md) · [Control map](./api_guide/en/19_control_map.md)

---

## 13. End-to-end control map

| Entry | Suggested data flow |
|-------|---------------------|
| Touch UI | `hub_model_*` → (optional) `app_bus_bridge_post_write` → Modbus |
| Bemfa MQTT | In-library points → **same bridge** write → `schedule_sync` |
| BLE SET_STATE | `apply_set_state` → model → same bridge |
| Ethernet TCP | Custom parse → `app_modbus_*` → refresh model |
| Scene one-shot | `apply_scene` → batched queue writes |
| AHT20 | `ui_bg_task` → `indoor_c`/`rh` → theme refresh (read-only sensing) |

Product direction picker: [G00](./api_guide/en/guides/G00_directions.md)

---

## 14. Secondary-development checklist

- [ ] Project root is `customer_sdk/`, target `esp32s3`, IDF 5.5.x  
- [ ] `APP_UI_THEME_ID` is one of the ten Hub themes  
- [ ] `flash` includes storage; `hub_ico` cache count looks normal  
- [ ] AHT20: log `AHT20 ready`; home T/H shows live values; offset calibrated via `aht20_calib.h`  
- [ ] RS485: UART conflict resolved; `read_holding` works without UI; buttons only enqueue  
- [ ] Wi‑Fi / MQTT / BLE integrated as needed; all entries share `hub_model`  
- [ ] No edits / reverse-engineering of `.a` libraries  

---

## 15. FAQ

**Q: UI text shows but icons are blank?**  
PNG + POSIX FS + storage flashed + `icons/nt/*.png` present in SPIFFS.

**Q: Home T/H stuck on `--`?**  
Check module soldering, `AHT20_ENABLE`, log `AHT20 ready`, and that new firmware was flashed.

**Q: Fixed offset vs reference meter?**  
Edit OFFSET/SCALE in `main/board/aht20_calib.h` and reflash; do not subtract again in the theme.

**Q: Where are RS485 headers?**  
`main/app/app_rs485.h`, `app_modbus_rtu.h`, or `#include "app_api.h"`.

**Q: Button press freezes UI?**  
Move `app_modbus_*` out of LVGL callbacks; use the §12.4 bridge.

**Q: Theme macro changed but UI is still old?**  
`idf.py reconfigure`; `fullclean` if needed.

**Q: Can we change backlight GPIO?**  
Pins live inside `board_bsp.a`; public API only exposes brightness percent. Board change needs a new BSP from the vendor.

**Q: Can MQTT UID/topics be changed?**  
Inside closed `cloud_wifi.a`; without a config API, ask the vendor or build your own MQTT client.

**Q: Link error missing `bt_switch_control_linker_keep`?**  
Confirm `main/hub_ui/ui_runtime_stubs.c` is in the build (exports that symbol).

**Q: CMake Missing required kconfig (LIBJPEG/LIBPNG/LZ4)?**  
See repo-root `tools/pin_esp_lvgl_port_lvgl.py` (pins esp_lvgl_port → lvgl 8.4.* at configure). Re-run `idf.py reconfigure` after failure.

---

## 16. Quick path index

```
customer_sdk/
  docs/THEME_SECONDARY_DEV.md        # Chinese overview
  docs/THEME_SECONDARY_DEV_EN.md     # English overview
  docs/api_guide/en/                 # chapters + G00–G06
  main/app_ui_theme_select.h
  main/app/app_api.h                 # open peripheral umbrella
  main/app/examples/                 # bus-bridge sample
  main/board/aht20_calib.h           # ★ T/H calibration
  main/gui/ui_bg_task.c              # AHT20 poll
  main/hub_ui/                       # shell / icons / ui_runtime stubs
  ui/themes/slate/home.c             # home T/H (default theme)
  spiffs_image/icons/nt/
  components/hub_core/include/hub_model.h
  components/cloud_wifi/include/
  components/bt_ctrl/include/
  components/board_bsp/include/aht20.h
```

---

## 17. Suggested learning order

1. Build/flash default `SLATE`; confirm icons, touch, home T/H (when module present).  
2. Change only `palette.c` for colors; tune `aht20_calib.h` against a reference meter.  
3. Read `hub_model.h`; button callbacks update widgets + `hub_ui_refresh()`.  
4. Prove `app_modbus_read_holding` without UI → wire `examples` bus bridge ([G01](./api_guide/en/guides/G01_modbus_rs485_lvgl.md)).  
5. Bring up Wi‑Fi + (optional) Bemfa MQTT; cloud and touch share the bridge.  
6. Enable BLE/Mesh / Ethernet gateway as needed ([G00](./api_guide/en/guides/G00_directions.md)).  

After these steps you can customize UI, calibrate T/H, and extend RS485 field control without touching closed sources.

When using AI assistants: pin [AI_CONTEXT.md](./api_guide/en/AI_CONTEXT.md); workflow in [AI_DEV.md](./api_guide/en/AI_DEV.md).
