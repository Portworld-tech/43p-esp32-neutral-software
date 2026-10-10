# SM43P / 4" ESP32 Panel — Customer FAQ (English)

For purchasing, hardware integration, and secondary-development engineers.  
Product form factor: **ESP32-S3 + 4" 480×480 touch panel (SM43P class) + neutral Hub SDK**.

Chinese edition: [CUSTOMER_FAQ_CN.md](./CUSTOMER_FAQ_CN.md) · Formatted mirror: [CUSTOMER_FAQ.md](./CUSTOMER_FAQ.md)  
Deeper secondary-dev docs: [THEME_SECONDARY_DEV.md](./THEME_SECONDARY_DEV.md), [CUSTOMER_API_GUIDE.md](./CUSTOMER_API_GUIDE.md).

---

## 0. One-minute summary

| Topic | Answer |
|------|--------|
| Development environment | **Recommended: VS Code / Cursor + ESP-IDF 5.5.x**; Arduino is only suitable for small demos |
| Delivery model | Neutral layered SDK: closed board/cloud/model libraries + open themes and business glue |
| Temperature / humidity | Optional **AHT20**, shared with touch on **I2C (IO7 = SCL, IO15 = SDA)**; plug in the module, enable `AHT20_ENABLE`, and values are read/displayed automatically; correct display offset via macros in `main/board/aht20_calib.h` (see §1.13) |
| Buzzer / RTC | Supported on the PCB; buzzer can provide key-click sound; RTC battery pad is supported (battery often not fitted at shipment) |
| RS-485 | Standard half-duplex; default **8N1** / Modbus; **MARK/SPACE stick parity is not a standard feature** (see §1.14) |
| GUI | **LVGL + Hub themes + ESP-IDF**; design with SquareLine / GUI Guider, etc. (see §1.15) |
| Home Assistant | No official out-of-the-box HA integration package; protocol-level integration (MQTT, etc.) is possible; vendor customization available |
| Icons | No built-in icon editor; export PNGs from iconfont or design tools and replace assets |

---

## 1. Answered questions (summary)

### 1.1 After installing the onboard temperature/humidity module, do we need to retune the software?

**Usually no.**  
The neutral firmware already wires AHT20 acquisition into the UI path. With the module installed correctly and `CONFIG_AHT20_ENABLE` enabled, a background task polls temperature/humidity, writes `hub_model`, and Hub themes refresh automatically.

> If you use your own firmware and not this SDK, you must integrate the driver yourself.

---

### 1.2 Is Arduino development supported?

| Scenario | Recommendation |
|----------|----------------|
| Small feature checks (Wi‑Fi scan, toggle buzzer) | Arduino / PlatformIO is OK for a **small demo** |
| Full smart-home product / production UI on this panel | **Not recommended**; use **VS Code + ESP-IDF** |

Reason: this product depends on ESP-IDF components, LVGL display, PSRAM, dual-core tasks, and layered `.a` libraries. The Arduino abstraction layer cannot stably carry the full project.

---

### 1.3 Is there an RTC battery pad on the board?

**Yes.** The board reserves an RTC battery pad and **hardware supports** an external battery for the clock domain.  
Shipments commonly leave the RTC battery **unpopulated**; fit one when you need timekeeping across power loss (battery type per the hardware manual).

---

### 1.4 Can we add a small buzzer for touch sound feedback?

**Yes.** The development board supports driving a **small active/passive piezoelectric buzzer** with a digital output (not a power-amplified speaker solution).  
Software uses public APIs such as `board_beep_set` / `app_beep_*` (see delivered headers). The settings page can also enable/disable key sounds.

---

### 1.5 Top connector / I2C / temperature-humidity / analog inputs

| Question | Answer |
|----------|--------|
| Purpose of the top connector | In the current delivery context, **I2C is used for temperature/humidity and similar peripherals** (shared-bus policy with touch: see board notes) |
| Is it I2C? | **Yes, I2C** |
| Can we connect a T/H sensor? | **Yes**; **AHT20** is recommended (shared bus with touch) |
| T/H pins | **SCL = IO7, SDA = IO15** |
| Analog temperature probe | This neutral SDK **does not treat ADC temperature probes as a standard delivery path**; NTC and similar need spare ADC pins / hardware revision assessment (customization) |

---

### 1.6 Can it work with Home Assistant (HA)?

| Item | Answer |
|------|--------|
| Official HA plugin? | **Not currently** — no dedicated out-of-the-box HA integration package |
| Compatible? | **Yes**: the device can talk to HA via **MQTT / HTTP**, etc. |
| Vendor capability | Neutral software is not locked to one cloud; similar cloud / mini-program projects have been done; **HA integration can be customized per project** |

Short-term path: device → Wi‑Fi → MQTT broker → HA MQTT integration. Longer term: vendor can provide entities / discovery templates.

---

### 1.7 Where is the temperature/humidity sensor connected? Which pins?

- Module: **AHT20** (optional plug-in / onboard depending on order)  
- Bus: shared **I2C** with touch  
- Pins: **IO7 (SCL), IO15 (SDA)**  
- Address: commonly `0x38` (see `menuconfig` / `AHT20_I2C_ADDR`)

---

### 1.8 How do we read T/H with Arduino / PlatformIO? Is there a guide?

**Official delivery does not use Arduino / PlatformIO as the main path.**

Please use:

1. **VS Code + ESP-IDF**  
2. This repository’s **neutral Customer SDK**  
3. Enable `AHT20_ENABLE` in `menuconfig`  
4. Flash a full firmware image including SPIFFS  

After the sensor is plugged in, a background task acquires data automatically. You can also call from the open layer:

```c
#include "aht20.h"
#include "withthewind_board_lvgl_init.h"

aht20_init(board_i2c_get_handle());

float t_c = 0.0f, rh = 0.0f;
aht20_read(&t_c, &rh);              /* calibrated: °C + %RH */
aht20_read_raw(&t_c, &rh);          /* uncalibrated raw (for tuning OFFSET) */
/* or */
aht20_read_temperature_c(&t_c);
aht20_read_humidity_rh(&rh);

/* Same data as the UI (calibrated aht20_read path) */
hub_model()->indoor_c;   /* °C */
hub_model()->rh;         /* integer %RH */
```

For field offset, edit macros in `main/board/aht20_calib.h`, then rebuild and flash (see **§1.13**).  
Without a sensor, the UI may show demo defaults — that is expected.

---

### 1.9 Do you provide ESP-IDF drivers/source and other development options?

| Provided | Not provided (or limited) |
|----------|---------------------------|
| Neutral Customer SDK (ESP-IDF project) | Full private product repository |
| Open: `ui/themes`, `hub_ui`, parts of `main/app`, AHT20 sources, etc. | Full GPIO / expander pin-map sources |
| Public headers + prebuilt `board_bsp` / `hub_core` / `cloud_wifi` / `bt_ctrl` | Reverse-engineering / redistributing closed `.c` |
| Ten Hub themes, docs, examples | SquareLine `default` theme pack (not in customer package) |

Customers can change UI/UX on the neutral software, connect RS485/Modbus/MQTT, and extend business logic.

---

### 1.10 Arduino is not enough — is there another option?

Yes — and this is the **recommended** path:

- **IDE**: VS Code / Cursor  
- **Framework**: **ESP-IDF 5.5.x**  
- **Project**: Customer SDK in this tree (`idf.py set-target esp32s3` → `build` → `flash`)  
- **UI preview (browser)**: in-repo / GitHub Pages `lvgl-front` theme picker  

Arduino is only for tiny demos, not a full smart-home project on this panel.

---

### 1.11 Is there a system/tool for creating icons?

**No built-in icon editor GUI.**

Recommended flow:

1. Export icons from [iconfont](https://www.iconfont.cn/) or Figma / Illustrator, etc.  
2. Make **white glyph + transparent background, ~96×96 PNG**  
3. Overwrite `spiffs_image/icons/nt/<name>.png` (names must match `hub_icons.c`)  
4. `idf.py build flash` (must program **storage/SPIFFS**)

Optional: vendor scripts can batch-generate matching mask PNGs from `lvgl-front` vector definitions.

---

### 1.12 How do we manage the display (backlight / theme / standby)?

There is no separate “display OS” — use **board APIs + Hub UI**:

| Need | How |
|------|-----|
| Backlight on/off / brightness | `board_backlight_on/off`, `board_backlight_set(0..100)` |
| Settings-page brightness | Already wired via `hub_device_*` / settings slider |
| Change theme / pages | Edit `main/app_ui_theme_select.h`; pages in `ui/themes/<theme>/` and `main/hub_ui/` |
| Standby dimming | `UI_AMBIENT_*` + `app_low_power` (see docs) |

Content is driven by **LVGL + `hub_model`**. Customers change themes or model data; no need to edit closed BSP pin sources.

---

### 1.13 How do we read temperature and humidity? How do we calibrate with macros?

**Automatic path (recommended)**

1. Hardware: AHT20 on I2C (IO7/IO15)  
2. Software: `CONFIG_AHT20_ENABLE=y`  
3. API: `aht20_read` (calibrated) / `aht20_read_raw` (uncalibrated); UI fields `hub_model()->indoor_c` / `rh`  
4. UI: Hub home / energy-related pages bind these fields and refresh to live values when a sensor is present  

**Customer macro calibration (open layer; rebuild + flash after edits)**

Edit: **`main/board/aht20_calib.h`**.

Formula:

```text
temp_out (°C) = temp_raw × AHT20_TEMP_SCALE + AHT20_TEMP_OFFSET_C
rh_out  (%)   = rh_raw   × AHT20_RH_SCALE   + AHT20_RH_OFFSET_PCT
```

| Macro | Default | Meaning |
|-------|---------|---------|
| `AHT20_CALIB_ENABLE` | `1` | `0` = disable all calibration (sensor math only) |
| `AHT20_TEMP_OFFSET_C` | `0.0f` | Temperature offset (°C); use negative if reading high |
| `AHT20_TEMP_SCALE` | `1.0f` | Temperature gain |
| `AHT20_RH_OFFSET_PCT` | `0.0f` | Humidity offset (%RH) |
| `AHT20_RH_SCALE` | `1.0f` | Humidity gain |
| `AHT20_RH_CLAMP` | `1` | Clamp RH to 0..100 after calibration |

Example (UI reads 1.5 °C high and 3 %RH low vs a reference meter):

```c
/* main/board/aht20_calib.h */
#define AHT20_TEMP_OFFSET_C   (-1.5f)
#define AHT20_RH_OFFSET_PCT   (3.0f)
```

Tuning tip: compare `aht20_read_raw` to a reference → adjust OFFSET/SCALE → `idf.py build flash` → confirm UI.  
Calibration applies in the driver layer so Hub UI, background polling, and customer `aht20_read*` calls stay consistent — do **not** subtract the same offset again in the theme.

---

### 1.14 Does RS-485 support MARK and SPACE modes?

Customers often mean two different things — align first:

| Meaning | SM43P / neutral SDK | Position |
|---------|---------------------|----------|
| **Electrical** idle Mark / active Space (differential A/B) | Standard RS-485 half-duplex + DE/RE | **Supported** (inherent to the PHY) |
| **Frame format** stick parity: address byte Mark, data byte Space (9th-bit multi-drop) | Default **8N1**; ESP-IDF UART exposes none/even/odd; `app_rs485` **does not expose** MARK/SPACE | **Not standard / not out-of-box**; evaluate customization if mandatory |

Open-layer status:

- `app_rs485_init` default: `UART_DATA_8_BITS` + `UART_PARITY_DISABLE` + `UART_STOP_BITS_1`; baud is configurable (often 115200 / 9600)  
- Typical **Modbus RTU** / custom 8-bit frames: use `app_rs485_*` / `app_modbus_*`  
- Details: `main/app/app_rs485.h` and [THEME_SECONDARY_DEV.md](./THEME_SECONDARY_DEV.md) §12

**Customer-facing reply:**

> The RS-485 port on the SM43P is a standard half-duplex electrical interface (UART + DE/RE). At the physical layer it follows RS-485 differential signaling (idle bus is in the Mark state).  
> On the software side, our neutral SDK defaults to **8N1** half-duplex communication and supports common usages such as Modbus RTU. Baud rate and related parameters can be configured in the open application layer.  
> If you mean UART **MARK/SPACE stick parity** (using the 9th bit to distinguish address vs. data bytes): that mode is **not exposed as a standard feature** in the delivered firmware or customer APIs. The ESP32-S3 ESP-IDF UART configuration primarily supports no / even / odd parity. If your field protocol strictly requires MARK/SPACE, please share the protocol specification and we can evaluate a custom implementation. Typical Modbus or custom 8-bit frames do not depend on that mode and can be integrated as-is.

> **Internal note:** Do not claim “hardware fully supports MARK/SPACE UART mode” without distinguishing electrical Mark/Space from stick parity; ask for the customer’s protocol document.

---

### 1.15 How is the GUI implemented? How do we create app UI? Which tools/frameworks/libraries?

| Layer | Technology |
|-------|------------|
| Graphics library | **LVGL** (on-device rendering) |
| System / build | **ESP-IDF 5.5.x** (VS Code / Cursor recommended) |
| Display / touch | Board BSP wrappers (e.g. ST7701 RGB **480×480**, GT911); apps do not edit closed pin maps |
| Business data | `hub_model` (temperature, rooms, scenes, protocol status, …) |
| UI shell / routing | `main/hub_ui/` |
| Look & layout | Ten open **Hub themes** under `ui/themes/<theme>/` |
| Assets | SPIFFS icons, etc. (must flash **storage**) |
| Theme preview (optional) | Browser **lvgl-front** / GitHub Pages; production UI follows in-firmware LVGL theme sources |

**How to create / modify the application UI:**

1. Environment: VS Code / Cursor + ESP-IDF (Arduino not recommended for a full product)  
2. Select a theme in `app_ui_theme_select.h`, or create one from `_template`  
3. Edit theme pages (e.g. `home.c`, `pages_*.c`) and colors (`palette.c`) with the LVGL API  
4. Put icons/assets in SPIFFS (e.g. PNG); flash must include the storage partition  
5. Optional: browser `lvgl-front` for theme selection preview  

**Tools / frameworks / libraries:** ESP-IDF, LVGL, `esp_lvgl_port`, board LCD/touch components; application stack is the neutral Hub SDK. The customer package does **not** include a SquareLine default project; UI is primarily designed with standalone desktop editors such as **SquareLine / GUI Guider**.

Docs: [THEME_SECONDARY_DEV.md](./THEME_SECONDARY_DEV.md)

**Customer-facing reply:**

> The SM43P on-device GUI is implemented with **LVGL**, running on **ESP-IDF** (recommended **5.5.x**). The display driver (e.g. ST7701 RGB 480×480) and touch controller (e.g. GT911) are wrapped by the board-level BSP, so applications do not modify low-level pin maps directly.  
>  
> **UI architecture:** business data in `hub_model`; UI shell/routing in `hub_ui`; look & layout via ten open Hub themes (`ui/themes/<theme_name>/`).  
> **How to create/modify:** VS Code + ESP-IDF; select/create a theme; edit pages and colors; put assets in SPIFFS and flash storage; optional `lvgl-front` preview.  
> **Tools / frameworks / libraries:** ESP-IDF, LVGL, `esp_lvgl_port`, board LCD/touch components, neutral Hub SDK. The customer package does not include SquareLine default; UI work is primarily done with standalone desktop GUI editors such as SquareLine or GUI Guider.

---

## 2. Likely follow-up questions (predicted + suggested answers)

### 2.1 Hardware & interfaces

| # | Likely question | Suggested answer |
|---|-----------------|------------------|
| P1 | Flash / PSRAM size? | Per datasheet (common **16MB Flash + 8MB PSRAM**); dual OTA partitions in `partitions.csv` |
| P2 | Ethernet? | Optional CH390 SPI Ethernet (board wrapper); watch memory with Wi‑Fi/BLE coexistence |
| P3 | USB download or Host? | **Download port** |
| P4 | External camera / speaker? | Non-standard; needs hardware assessment and custom software |
| P5 | RS485 pins / isolation? | Open-layer UART/RS485 sample framework; **silkscreen/isolation per hardware manual** — do not assume full pinmux sources are in the customer package |
| P5b | RS485 MARK/SPACE? | See **§1.14**: electrical Mark/Space yes; stick parity not standard; default 8N1 / Modbus |
| P6 | Proximity sensor connector? | If I2C extension is reserved, prefer the shared I2C bus; driver by customer or customization |
| P7 | Voltage / power / certification? | Supply **3V3**; low power via `app_low_power` / `UI_AMBIENT_*` |

### 2.2 Software & delivery

| # | Likely question | Suggested answer |
|---|-----------------|------------------|
| P8 | Full source code? | **Neutral layered SDK**; board/cloud/model cores as `.a` + headers; themes and business code open |
| P9 | Relation to the public GitHub repo? | Public repo = shareable neutral sample/preview; production = contract SDK package |
| P10 | Can the ten themes be used commercially? | Neutral Hub themes for OEM secondary development; do not mix third-party exclusive UI assets |
| P11 | OTA? | Dual OTA partitions; upgrade channel (HTTP/MQTT) can be extended on IDF or customized |
| P12 | Bemfa cloud / own MQTT? | Closed library includes Bemfa path; own broker can be added in the open layer — isolate from default MQTT switches |
| P13 | Multi-language? | Hub model supports Chinese/English (`hub_model` settings) |
| P14 | Headless gateway without a display? | UI can theoretically be trimmed; current delivery is display-centric HMI — headless is custom |

### 2.3 Temperature / humidity & sensors

| # | Likely question | Suggested answer |
|---|-----------------|------------------|
| P15 | Accuracy / calibration? | Follow AHT20 specs; first edit **`main/board/aht20_calib.h`** macros (§1.13); extend to NVS if you need persistence across power cycles |
| P16 | Swap to SHT30 / DHT22? | Non-standard; needs driver change and bus assessment (DHT is not I2C) |
| P17 | Two temperatures: indoor vs setpoint? | `indoor_c`/`rh` = environment; AC setpoint is in room widgets / `hub_model` device fields |
| P18 | Plugged in but no display? | Check soldering/orientation, `AHT20_ENABLE`, I2C conflicts, log `AHT20 ready`, whether new firmware was flashed |

### 2.4 Display & UI

| # | Likely question | Suggested answer |
|---|-----------------|------------------|
| P19 | Resolution? | **480×480** |
| P20 | Ship with SquareLine as-is? | Customer package **does not** include SquareLine default; use ten Hub themes or build Hub-style pages; SquareLine / GUI Guider can design then land into themes |
| P20b | What is the GUI stack? How to change UI? | See **§1.15**: LVGL + Hub themes + ESP-IDF |
| P21 | Browser preview? | `lvgl-front`: `npx serve .` or GitHub Pages |
| P22 | Blank icons? | Must flash **storage**; check PNG + `LV_USE_PNG` + FS POSIX |
| P23 | Why does brightness jump in steps? | Older expander software PWM is stepped; newer GPIO LEDC can be smoother (depends on board revision) |

### 2.5 Cloud & ecosystem

| # | Likely question | Suggested answer |
|---|-----------------|------------------|
| P24 | WeChat mini-program / App? | Not mandatory; can be customized (similar capability has been done) |
| P25 | Remote ops without port mapping? | Device must **outbound** to cloud (MQTT/WSS) + cloud portal; LAN web is separate |
| P26 | Matter / HomeKit? | Not a neutral-SDK standard feature; needs dedicated assessment |
| P27 | Data security / accounts? | Neutral package focuses on device side; accounts/audit are cloud-side customization |

### 2.6 Commercial & support

| # | Likely question | Suggested answer |
|---|-----------------|------------------|
| P28 | MOQ / lead time / customization cycle? | Commercial terms (no fixed numbers in this doc) |
| P29 | Training & tech support? | Docs + SDK; deep bring-up can be under a support/custom contract |
| P30 | License / redistribution? | Do not reverse closed `.a`; open layer redistributable per contract for end-customer secondary development |

---

## 3. Sales / support one-liners

| Customer says | One-line reply |
|---------------|----------------|
| Do we retune software after plugging T/H? | No — neutral firmware already supports it; enable and it displays automatically. |
| Readings don’t match our reference meter? | Edit OFFSET/SCALE macros in `main/board/aht20_calib.h`, then rebuild and flash. |
| Can we do whole-home with Arduino? | Not suitable; use VS Code + ESP-IDF + neutral SDK. |
| Is there an RTC battery? | Pad is supported; battery often not fitted by default. |
| Need key-click buzzer? | Yes — board supports a small digital buzzer. |
| What is the top connector? | I2C; T/H uses IO7/IO15. |
| Can it work with HA? | No official plugin; MQTT etc. compatible; customization available. |
| How do we make icons? | Export PNG from iconfont etc. and replace SPIFFS assets. |
| How do we manage the screen? | Backlight APIs + theme macros + `hub_model` — not a separate display OS. |
| Does RS485 support MARK/SPACE? | Electrical yes; stick parity not standard. Default 8N1 / Modbus; stick parity needs custom evaluation. |
| How is the GUI built? | LVGL + ESP-IDF + Hub themes; design with SquareLine / GUI Guider. |
| Do we get all source? | Neutral layered SDK: themes open; board core libraries closed. |

---

## 4. Related documents

| Document | Content |
|----------|---------|
| [THEME_SECONDARY_DEV.md](./THEME_SECONDARY_DEV.md) | Overview, themes, icons, Wi‑Fi/MQTT, AHT20, RS485 |
| GitHub sample | [Portworld-tech/43p-esp32-neutral-software](https://github.com/Portworld-tech/43p-esp32-neutral-software) |
| Theme preview | [GitHub Pages](https://portworld-tech.github.io/43p-esp32-neutral-software/) |

---

## 5. Revision history

| Version | Date | Notes |
|---------|------|-------|
| 1.0 | 2026-10-09 | English edition aligned with `CUSTOMER_FAQ_CN.md` (incl. RS-485, GUI, AHT20 macro calibration) |
