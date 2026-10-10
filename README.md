# 43P ESP32 Neutral Software

Organization: **Portworld-tech**  
Contact: xjunsoftware@ycxytech.com

ESP32-S3 **customer / OEM layered SDK** for a 480脳480 Hub UI panel: closed board/cloud/BT libraries + open themes and `hub_ui`. Board pin maps are **not** published as source (baked into `board_bsp.a`).

## Firmware (this tree)

| Path | Role |
|------|------|
| `components/board_bsp` | Closed: display / touch / backlight (`.a`) + public headers |
| `components/hub_core` | Closed: `hub_model` |
| `components/cloud_wifi` | Closed: Wi鈥慒i + Bemfa MQTT |
| `components/bt_ctrl` | Closed: BLE / Mesh control |
| `main/hub_ui` | Open: UI chrome |
| `ui/themes/*` | Open: ten Hub themes |
| `spiffs_image` | Open: icons / assets |
| `docs/` | Secondary-dev & API guides |

### Quick start

1. Edit `main/app_ui_theme_select.h` 鈫?pick a Hub theme (`SLATE` / `SAND` / 鈥? not `DEFAULT`).
2. Customize `ui/themes/<id>/` or replace `spiffs_image/icons/nt/*.png`.
3. Call public APIs: `hub_model.h`, `wifi_management.h`, `wifi_bemfa_client.h`, `bt_management.h`, `board_*`.

```powershell
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

ESP-IDF **5.5.x**, target **esp32s3**. Flash must include the **storage (SPIFFS)** partition so icons appear.

Secondary-dev overview: [docs/THEME_SECONDARY_DEV.md](docs/THEME_SECONDARY_DEV.md) (Chinese) · [docs/THEME_SECONDARY_DEV_EN.md](docs/THEME_SECONDARY_DEV_EN.md) (English).

---

## UI preview (browser) 鈥?`lvgl-front`

Interactive **OEM Hub UI Kit** (480脳480): ten theme hubs, rooms, scenes, Wi鈥慒i sheet, schedules. Same visual language as the on-device themes; use it for customer theme selection before flashing firmware.

### Run locally

```bash
cd lvgl-front
npx serve .
```

Open the URL shown (needs network for React / Babel / fonts CDN).

| Theme | Style |
|-------|--------|
| **Slate** | Industrial deep gray / teal |
| **Sand** | Daytime warm stone |
| **Ink** | High-contrast debug / industrial |
| **Forest** | Green energy |
| **Dusk** | Evening glass / hospitality |
| **Ocean** | Ops / gateway blue |
| **Zen** | Minimal clock focus |
| **Pulse** | HUD / tech demo |
| **Bloom** | Soft home bubbles |
| **Metro** | Color tile mosaic |

Full kit notes: [lvgl-front/README.md](lvgl-front/README.md).

### Online demo (GitHub Pages)

After Pages is enabled on this repo (source: `/lvgl-front` or `docs` workflow), open:

**https://portworld-tech.github.io/43p-esp32-neutral-software/**

---


### AHT20 temperature and humidity

Enable `CONFIG_AHT20_ENABLE`. Open source driver: `main/board/aht20.c`.  
Field offset: edit macros in `main/board/aht20_calib.h` (`out = raw * SCALE + OFFSET`), then rebuild.

```c
#include "aht20.h"
float t = 0, rh = 0;
aht20_init(board_i2c_get_handle());
aht20_read(&t, &rh);       /* calibrated C and %RH */
aht20_read_raw(&t, &rh);   /* uncalibrated */
```

Background task writes `hub_model()->indoor_c` / `hub_model()->rh`; Hub themes refresh automatically.
