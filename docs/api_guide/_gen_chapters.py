# -*- coding: utf-8 -*-
"""Generate product-only ZH/EN API guide (Customer SDK secondary development)."""
from pathlib import Path

ROOT = Path(__file__).resolve().parent
ZH, EN = ROOT / "zh", ROOT / "en"

META = [
    ("01_overview", "概览与分层", "Overview & Layering", "start"),
    ("02_build", "环境与编译", "Build", "start"),
    ("03_boot", "启动顺序", "Boot Order", "start"),
    ("04_wifi", "Wi-Fi", "Wi-Fi", "net"),
    ("05_mqtt", "MQTT（巴法云）", "MQTT (Bemfa)", "net"),
    ("06_bt", "蓝牙 BLE / Mesh", "Bluetooth", "net"),
    ("07_eth", "以太网", "Ethernet", "net"),
    ("08_backlight", "背光", "Backlight", "hw"),
    ("09_beep", "蜂鸣器", "Buzzer", "hw"),
    ("10_gpio_rs485", "GPIO_OUT / RS485", "GPIO_OUT / RS485", "hw"),
    ("11_modbus", "Modbus RTU", "Modbus RTU", "hw"),
    ("12_coexist", "射频共存", "Coexistence", "hw"),
    ("13_low_power", "低功耗", "Low Power", "sys"),
    ("14_gui_task", "跨线程 GUI", "GUI Task", "sys"),
    ("15_hub", "Hub 模型与 UI", "Hub Model & UI", "ui"),
    ("16_icons", "图标更换", "Icons", "ui"),
    ("17_health", "健康监控", "Health", "sys"),
    ("18_kconfig", "Kconfig", "Kconfig", "ref"),
    ("19_control_map", "控制通路", "Control Map", "ref"),
    ("20_faq", "FAQ", "FAQ", "ref"),
    ("A_headers", "头文件索引", "Header Index", "ref"),
]

CAT = {
    "zh": {"start": "入门", "net": "网络与云", "hw": "外设与总线", "sys": "系统", "ui": "界面", "ref": "参考"},
    "en": {"start": "Start", "net": "Network", "hw": "Peripherals", "sys": "System", "ui": "UI", "ref": "Reference"},
}

# Hand-maintained (ZH + EN) — generator must not overwrite.
# EN docs are curated by hand (no script translation). Guides / AI_* are outside META.
HAND_WRITTEN = {
    "00_secondary_dev",
    "01_overview",
    "02_build",
    "03_boot",
    "04_wifi",
    "05_mqtt",
    "06_bt",
    "07_eth",
    "08_backlight",
    "09_beep",
    "10_gpio_rs485",
    "11_modbus",
    "12_coexist",
    "13_low_power",
    "14_gui_task",
    "15_hub",
    "16_icons",
    "17_health",
    "18_kconfig",
    "19_control_map",
    "20_faq",
    "A_headers",
    "README",
}


def nav(stem, lang):
    idx = next(i for i, m in enumerate(META) if m[0] == stem)
    parts = []
    if idx:
        p = META[idx - 1]
        parts.append(f"[← {p[1 if lang=='zh' else 2]}](./{p[0]}.md)")
    parts.append("[目录](./README.md)" if lang == "zh" else "[TOC](./README.md)")
    if idx < len(META) - 1:
        n = META[idx + 1]
        parts.append(f"[{n[1 if lang=='zh' else 2]} →](./{n[0]}.md)")
    return " | ".join(parts)


def w(lang, stem, body):
    if stem in HAND_WRITTEN:
        return
    note = (
        "\n\n> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。\n"
        if lang == "zh"
        else "\n\n> Canonical: `include/*.h` / `main/app/*.h`. Do not reverse closed `.a` libs.\n"
    )
    (ZH if lang == "zh" else EN).joinpath(f"{stem}.md").write_text(
        body.rstrip() + f"\n\n---\n\n{nav(stem, lang)}" + note, encoding="utf-8"
    )


def all_chapters():
    # 01
    w("zh", "01_overview", r'''# 1. 概览与分层

> **本手册仅描述 Customer SDK（本工程）二次开发 API。** 硬件测试台 `esp32_s3_frame` 的 UI 不在交付范围内；其验证过的能力已移植到 `main/app/` 开放层。

## 分层

| 层 | 路径 | 说明 |
|----|------|------|
| 开放业务 | `main/`、`ui/themes/`、`spiffs_image/` | 可改源码 |
| 开放外设 API | `main/app/app_*.h` | 蜂鸣脉冲、RS485、Modbus、共存等 |
| 封闭库 | `components/*/include` + `.a` | Wi-Fi/BT/Hub/板级驱动 |

统一入口：

```c
#include "app_api.h"   /* beep / coexist / rs485 / modbus / wifi_util / gpio_out */
```

## 相关

[编译](./02_build.md) · [启动](./03_boot.md) · [头文件](./A_headers.md)
''')
    w("en", "01_overview", r'''# 1. Overview & Layering

> **This guide covers Customer SDK APIs only.** The `esp32_s3_frame` testbench UI is not a customer deliverable; proven helpers were ported into `main/app/`.

## Layers

| Layer | Path | Notes |
|-------|------|-------|
| Open app/UI | `main/`, `ui/themes/`, `spiffs_image/` | Editable |
| Open peripheral APIs | `main/app/app_*.h` | Beep pulse, RS485, Modbus, coexist |
| Closed libs | `components/*/include` + `.a` | Wi-Fi/BT/Hub/board |

```c
#include "app_api.h"
```

## See also

[Build](./02_build.md) · [Boot](./03_boot.md) · [Headers](./A_headers.md)
''')

    # 02
    w("zh", "02_build", r'''# 2. 环境与编译

- ESP-IDF **5.5.x** · 目标 `esp32s3`
- `idf.py build` / `flash` **必须含 SPIFFS storage**

```powershell
cd customer_sdk
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

RS485 若占用 UART0：将控制台改为 **USB Serial/JTAG**（与测试验证一致），或自定义 `app_rs485_config_t` 换口。
''')
    w("en", "02_build", r'''# 2. Build

- ESP-IDF **5.5.x** · target `esp32s3`
- Flash must include SPIFFS `storage`

```powershell
cd customer_sdk
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

If RS485 uses UART0, switch console to **USB Serial/JTAG** or pass a custom `app_rs485_config_t`.
''')

    # 03
    w("zh", "03_boot", r'''# 3. 启动顺序

```c
wifi_management_foundation_init();
app_low_power_init();
bt_management_init(...);          /* 可选 */
app_spiffs_mount();
board_display_start_with_lvgl_cfg(...);
board_ethernet_ch390_init/try_start();
board_backlight_init();
/* 可选: app_rs485_init(NULL)  — Kconfig APP_RS485_AUTO_INIT */
app_ui_start();
gui_task_init();
wifi_management_start();
bt_management_start();            /* 可选 */
```

显示初始化之后才有 I2C/扩展器，RS485 DE（GPIO_OUT）依赖此顺序。
''')
    w("en", "03_boot", r'''# 3. Boot Order

```c
wifi_management_foundation_init();
app_low_power_init();
bt_management_init(...);          /* optional */
app_spiffs_mount();
board_display_start_with_lvgl_cfg(...);
board_ethernet_ch390_init/try_start();
board_backlight_init();
/* optional: app_rs485_init(NULL) via APP_RS485_AUTO_INIT */
app_ui_start();
gui_task_init();
wifi_management_start();
bt_management_start();
```

Expander (RS485 DE) is ready only after display bring-up.
''')

    # 04 wifi
    w("zh", "04_wifi", r'''# 4. Wi-Fi

```c
#include "wifi_management.h"
#include "app_wifi_util.h"
```

| API | 作用 |
|-----|------|
| `wifi_management_foundation_init/start` | 基础 / STA |
| `wifi_management_connect/disconnect_user` | 连接 / 断开 |
| `wifi_management_scan_blocking` | 阻塞扫描 |
| `wifi_management_is_connected` | 是否获 IP |
| `app_wifi_scan_sorted` | 扫描并按 RSSI 排序 |
| `app_wifi_format_status` | `"SSID \| IP \| RSSI"` |

```c
wifi_ap_record_t aps[16];
uint16_t n = 16;
app_wifi_scan_sorted(aps, &n);
wifi_management_connect("MySSID", "pwd");
```
''')
    w("en", "04_wifi", r'''# 4. Wi-Fi

```c
#include "wifi_management.h"
#include "app_wifi_util.h"
```

| API | Purpose |
|-----|---------|
| `wifi_management_foundation_init/start` | Foundation / STA |
| `wifi_management_connect/disconnect_user` | Connect / disconnect |
| `wifi_management_scan_blocking` | Blocking scan |
| `app_wifi_scan_sorted` | Scan + RSSI sort |
| `app_wifi_format_status` | Status string |
''')

    # 05 mqtt
    w("zh", "05_mqtt", r'''# 5. MQTT（巴法云）

```c
#include "wifi_bemfa_client.h"
```

| API | 作用 |
|-----|------|
| `wifi_bemfa_client_start/stop` | 启停（GOT_IP 时常自动 start） |
| `wifi_bemfa_client_publish_status_u8` | 单点应答 |
| `wifi_bemfa_client_schedule_sync` | 全量快照 |

需 `APP_FEATURE_MQTT=y`。本地改态后：`hub_model_*` → `hub_ui_refresh()` → `schedule_sync()`。
''')
    w("en", "05_mqtt", r'''# 5. MQTT (Bemfa)

```c
#include "wifi_bemfa_client.h"
```

Enable `APP_FEATURE_MQTT`. After local changes: `hub_model_*` → `hub_ui_refresh()` → `schedule_sync()`.
''')

    # 06 bt
    w("zh", "06_bt", r'''# 6. 蓝牙 BLE / Mesh

```c
#include "bt_management.h"
#include "bt_proto.h"
#include "app_coexist.h"
```

扫描前：

```c
app_coexist_before_ble_scan();
bt_management_ble_scan_start(5);
/* … done … */
app_coexist_after_ble_scan();
```

覆盖点位：实现非 weak 的 `bt_management_apply_set_state(item_id, value)`，内部改 `hub_model` 并 `hub_ui_refresh()`。
''')
    w("en", "06_bt", r'''# 6. Bluetooth

```c
#include "bt_management.h"
#include "app_coexist.h"
```

Wrap BLE scans with `app_coexist_before/after_ble_scan()`. Override weak `bt_management_apply_set_state`.
''')

    # 07 eth
    w("zh", "07_eth", r'''# 7. 以太网

```c
#include "board_ethernet_ch390.h"
#include "app_coexist.h"
```

| API | 作用 |
|-----|------|
| `board_ethernet_ch390_init/try_start` | 初始化 / 启动 |
| `get_ip` / `link_up` / `get_link_info` | 状态 |
| `set_traffic_paused` | 暂停 SPI（RS485 突发时由 `app_coexist_*_rs485_burst` 调用） |

重负载前：`app_coexist_before_ethernet_work()`。
''')
    w("en", "07_eth", r'''# 7. Ethernet

```c
#include "board_ethernet_ch390.h"
#include "app_coexist.h"
```

Use `app_coexist_before_ethernet_work()` before heavy ETH work; RS485 bursts auto-pause SPI via coexist helpers.
''')

    # 08 backlight
    w("zh", "08_backlight", r'''# 8. 背光

```c
#include "withthewind_board_lvgl_init.h"
#include "hub_device_ui.h"
```

`board_backlight_init` → 首帧后 `board_backlight_on/set(0..100)`。设置页：`hub_device_brightness_get/apply_effective`。
''')
    w("en", "08_backlight", r'''# 8. Backlight

`board_backlight_init` then after first frame `on/set(0..100)`. Settings: `hub_device_brightness_*`.
''')

    # 09 beep
    w("zh", "09_beep", r'''# 9. 蜂鸣器

```c
#include "app_beep.h"
/* 底层 */
#include "withthewind_board_lvgl_init.h"  /* board_beep_set */
```

| API | 作用 |
|-----|------|
| `board_beep_set(on)` | 直接开关 |
| `app_beep_pulse(ms)` | 单次脉冲（esp_timer） |
| `app_beep_pulse_n(n, on, off)` | 连响 |
| `app_beep_click_if_enabled()` | 尊重 `hub_model` 按键音开关 |

```c
app_beep_pulse(120);
app_beep_click_if_enabled();
```
''')
    w("en", "09_beep", r'''# 9. Buzzer

```c
#include "app_beep.h"
```

`app_beep_pulse` / `pulse_n` use `esp_timer` (no LVGL). `app_beep_click_if_enabled` respects Hub click-sound setting.
''')

    # 10 gpio/rs485
    w("zh", "10_gpio_rs485", r'''# 10. GPIO_OUT / RS485

> 从测试台移植的**无 UI** 传输层，供二次开发直接调用。

## GPIO_OUT / DE

```c
#include "app_gpio_out.h"
#include "board_io_expander.h"

app_gpio_out_set(true);   /* 高 = RS485 发送 */
app_rs485_de_set(false);  /* 别名 */
```

## RS485

```c
#include "app_rs485.h"

app_rs485_init(NULL);   /* 默认 UART0 TX43/RX44 @115200 + 扩展器 DE */
const uint8_t msg[] = "HELLO\r\n";
app_rs485_write(msg, sizeof(msg) - 1, 1000);

uint8_t rx[128];
int n = 0;
app_rs485_transact(msg, sizeof(msg) - 1, rx, sizeof(rx), &n, 0, 700);
app_rs485_update_hub_health(n > 0, n > 0 ? 90 : 10, true);
```

自定义引脚：`app_rs485_get_default_config` → 改字段 → `app_rs485_init(&cfg)`。

**注意：** 勿在 LVGL 线程长时间阻塞；控制台占用 UART0 时先改 USB-JTAG 或换 UART。  
Kconfig：`APP_ENABLE_RS485`、`APP_RS485_AUTO_INIT`（默认关）。
''')
    w("en", "10_gpio_rs485", r'''# 10. GPIO_OUT / RS485

UI-free transport ported from the testbench for secondary development.

```c
#include "app_rs485.h"
#include "app_gpio_out.h"

app_rs485_init(NULL);  /* UART0 GPIO43/44 @115200 + expander DE */
app_rs485_write(...);
app_rs485_transact(...);
app_rs485_update_hub_health(ok, health, true);
```

Do not block the LVGL task. Free UART0 (USB-JTAG console) or pass custom pins.  
Kconfig: `APP_RS485_AUTO_INIT` defaults **off**.
''')

    # 11 modbus
    w("zh", "11_modbus", r'''# 11. Modbus RTU

```c
#include "app_modbus_rtu.h"

uint16_t regs[2];
esp_err_t err = app_modbus_read_holding(0x01, 0x0000, 1, regs, 1, 700);
err = app_modbus_write_single(0x01, 0x0001, 0x00FF, 700);
```

依赖 `app_rs485`。CRC：`app_modbus_crc16`。
''')
    w("en", "11_modbus", r'''# 11. Modbus RTU

```c
#include "app_modbus_rtu.h"
app_modbus_read_holding(0x01, 0x0000, 1, regs, 1, 700);
app_modbus_write_single(0x01, 0x0001, 0x00FF, 700);
```

Requires `app_rs485`.
''')

    # 12 coexist
    w("zh", "12_coexist", r'''# 12. 射频共存

```c
#include "app_coexist.h"
```

| API | 时机 |
|-----|------|
| `app_coexist_before/after_ble_scan` | BLE 扫描 |
| `app_coexist_before_ethernet_work` | 以太网重负载 |
| `app_coexist_before/after_rs485_burst` | RS485 突发（`app_rs485_write` 内已调用） |
| `app_coexist_heap_ok_for_eth` | DMA 堆门禁 |
''')
    w("en", "12_coexist", r'''# 12. Coexistence

```c
#include "app_coexist.h"
```

Wrap BLE scans and heavy ETH; RS485 write already pauses CH390 SPI.
''')

    # 13 lp
    w("zh", "13_low_power", r'''# 13. 低功耗

```c
#include "app_low_power.h"
app_low_power_init();  /* foundation 之后、显示之前 */
```

RGB 场景下默认不开 Light sleep；待机用 `UI_AMBIENT_*` 降背光。
''')
    w("en", "13_low_power", r'''# 13. Low Power

`app_low_power_init()` after net foundation. Light sleep stays off under RGB; use ambient backlight dim.
''')

    # 14 gui
    w("zh", "14_gui_task", r'''# 14. 跨线程 GUI

```c
#include "gui_task.h"
gui_task_post_lvgl(cb, data);
gui_task_post_lvgl_ex(cb, data, free);  /* 堆失败清理 */
```

RS485 / Wi-Fi 事件回调里更新控件必须走此路径。
''')
    w("en", "14_gui_task", r'''# 14. GUI Task

Post LVGL work from other tasks via `gui_task_post_lvgl` / `_ex`.
''')

    # 15 hub
    w("zh", "15_hub", r'''# 15. Hub 模型与 UI

```c
#include "hub_model.h"
#include "hub_ui.h"

hub_model_toggle_widget(room, slot);
hub_model_set_widget_level(room, slot, 80);
hub_ui_refresh();
```

主题：`main/app_ui_theme_select.h` 的 `APP_UI_THEME_ID`。必实现 `hub_theme_build`。
''')
    w("en", "15_hub", r'''# 15. Hub Model & UI

Mutate via `hub_model_*`, then `hub_ui_refresh()`. Themes selected by `APP_UI_THEME_ID`.
''')

    # 16 icons
    w("zh", "16_icons", r'''# 16. 图标更换

替换 `spiffs_image/icons/nt/*.png` → `idf.py flash`。  
API：`app_spiffs_mount`、`hub_ico_cache_init`、`hub_ico_add`。
''')
    w("en", "16_icons", r'''# 16. Icons

Replace `spiffs_image/icons/nt/*.png` and flash storage. APIs: `app_spiffs_mount`, `hub_ico_*`.
''')

    # 17 health
    w("zh", "17_health", r'''# 17. 健康监控

```c
#include "app_health.h"
app_health_monitor_start();
```
''')
    w("en", "17_health", r'''# 17. Health

`app_health_monitor_start()` for periodic heap/stack logs.
''')

    # 18 kconfig
    w("zh", "18_kconfig", r'''# 18. Kconfig

| 项 | 说明 |
|----|------|
| `APP_FEATURE_MQTT/BLE/MESH` | 云 / 蓝牙 |
| `APP_ENABLE_RS485` | 编译 RS485/Modbus API |
| `APP_RS485_AUTO_INIT` | 启动时 `app_rs485_init`（默认 n） |
| `UI_AMBIENT_*` | 待机背光 |
| `APP_HEALTH_MONITOR` | 堆监控 |
''')
    w("en", "18_kconfig", r'''# 18. Kconfig

`APP_FEATURE_*`, `APP_ENABLE_RS485`, `APP_RS485_AUTO_INIT` (default off), `UI_AMBIENT_*`, `APP_HEALTH_MONITOR`.
''')

    # 19 map
    w("zh", "19_control_map", r'''# 19. 控制通路

| 入口 | 建议 |
|------|------|
| UI | `hub_model_*` →（可选）`app_rs485` / `app_modbus_*` |
| MQTT | 库内点位 → 扩展外设写 |
| BLE | `apply_set_state` → model |
| 自研任务 | `app_rs485_transact` → model → `gui_task_post_lvgl` |
''')
    w("en", "19_control_map", r'''# 19. Control Map

UI/MQTT/BLE/custom tasks should converge on `hub_model_*` (+ optional `app_rs485` / Modbus).
''')

    # 20 faq
    w("zh", "20_faq", r'''# 20. FAQ

| 问题 | 处理 |
|------|------|
| 图标空白 | flash storage；PNG + POSIX FS |
| RS485 无响应 | 查 DE；控制台是否占 UART0；`app_rs485_is_ready` |
| 链接找不到 expander 符号 | 已恢复 `board_io_expander.h` 客户子集；确认链接 `board_bsp.a` |
| 按键无蜂鸣 | `app_beep_click_if_enabled` 或检查 click_sound |
''')
    w("en", "20_faq", r'''# 20. FAQ

Blank icons → flash SPIFFS. RS485 silent → DE + UART0 console conflict. Beep → `app_beep_*` / click_sound.
''')

    # A
    w("zh", "A_headers", r'''# 附录 A：头文件索引

## 开放层（二次开发优先）

| 功能 | 头文件 |
|------|--------|
| 总入口 | `main/app/app_api.h` |
| 蜂鸣脉冲 | `main/app/app_beep.h` |
| 共存 | `main/app/app_coexist.h` |
| GPIO_OUT/DE | `main/app/app_gpio_out.h` |
| RS485 | `main/app/app_rs485.h` |
| Modbus | `main/app/app_modbus_rtu.h` |
| Wi-Fi 工具 | `main/app/app_wifi_util.h` |
| 低功耗 | `main/app/app_low_power.h` |
| GUI 投递 | `main/gui/gui_task.h` |
| 扩展器（安全子集） | `components/board_bsp/include/board_io_expander.h` |

## 封闭库公开头

| 功能 | 头文件 |
|------|--------|
| Wi-Fi | `wifi_management.h` |
| MQTT | `wifi_bemfa_client.h` |
| 蓝牙 | `bt_management.h` / `bt_proto.h` |
| 以太网 | `board_ethernet_ch390.h` |
| 背光/蜂鸣 | `withthewind_board_lvgl_init.h` |
| Hub 模型 | `hub_model.h` |
''')
    w("en", "A_headers", r'''# Appendix A: Header Index

**Open:** `app_api.h`, `app_beep.h`, `app_coexist.h`, `app_gpio_out.h`, `app_rs485.h`, `app_modbus_rtu.h`, `app_wifi_util.h`, `gui_task.h`, `board_io_expander.h` (safe subset).

**Closed public:** `wifi_management.h`, `wifi_bemfa_client.h`, `bt_management.h`, `board_ethernet_ch390.h`, `withthewind_board_lvgl_init.h`, `hub_model.h`.
''')


def write_readmes():
    ROOT.joinpath("README.md").write_text(
        """# Customer SDK API Guide

Product secondary-development APIs only (open `main/app` + closed public headers).

| Lang | TOC |
|------|-----|
| 中文 | [zh/README.md](zh/README.md) |
| English | [en/README.md](en/README.md) |

Regenerate: `python docs/api_guide/_gen_chapters.py`
""",
        encoding="utf-8",
    )
    for lang in ("zh", "en"):
        lines = [
            "# Customer SDK API 手册\n" if lang == "zh" else "# Customer SDK API Guide\n",
            "> 仅本工程二次开发 API；测试台能力已并入 `main/app/`。\n"
            if lang == "zh"
            else "> Product APIs only; testbench helpers live under `main/app/`.\n",
            "## TOC\n",
        ]
        cur = None
        for stem, zh_t, en_t, cat in META:
            if cat != cur:
                cur = cat
                lines.append(f"\n### {CAT[lang][cat]}\n")
            lines.append(f"- [{zh_t if lang=='zh' else en_t}](./{stem}.md)")
        (ZH if lang == "zh" else EN).joinpath("README.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def main():
    ZH.mkdir(parents=True, exist_ok=True)
    EN.mkdir(parents=True, exist_ok=True)
    # Do NOT wipe hand-written chapters / guides / README
    all_chapters()
    # Keep existing zh/en README.md (hand-maintained with guides index)
    print("regenerated API chapters (skipped hand-written:", ", ".join(sorted(HAND_WRITTEN)), ")")
    print("zh", len(list(ZH.glob("*.md"))), "en", len(list(EN.glob("*.md"))))


if __name__ == "__main__":
    main()
