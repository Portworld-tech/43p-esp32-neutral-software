# 5. MQTT (Bemfa)

```c
#include "wifi_bemfa_client.h"
```

| API | Role |
|-----|------|
| `wifi_bemfa_client_start/stop` | Start/stop (often auto-starts on GOT_IP) |
| `wifi_bemfa_client_publish_status_u8` | Single-point reply |
| `wifi_bemfa_client_schedule_sync` | Full snapshot sync |

Requires `APP_FEATURE_MQTT=y`. After local state changes: `hub_model_*` → `hub_ui_refresh()` → `schedule_sync()`.

When also driving RS485, cloud commands must share the same bus bridge as touch ([G01](./guides/G01_modbus_rs485_lvgl.md) · [G02](./guides/G02_wifi_mqtt_panel.md)).

---

[← Wi-Fi](./04_wifi.md) | [README](./README.md) | [Bluetooth →](./06_bt.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
