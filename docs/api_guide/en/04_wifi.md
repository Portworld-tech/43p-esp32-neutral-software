# 4. Wi-Fi

```c
#include "wifi_management.h"
#include "app_wifi_util.h"
```

| API | Role |
|-----|------|
| `wifi_management_foundation_init/start` | Foundation / STA |
| `wifi_management_connect/disconnect_user` | Connect / disconnect |
| `wifi_management_scan_blocking` | Blocking scan |
| `wifi_management_is_connected` | Has IP |
| `app_wifi_scan_sorted` | Scan sorted by RSSI |
| `app_wifi_format_status` | `"SSID \| IP \| RSSI"` |

```c
wifi_ap_record_t aps[16];
uint16_t n = 16;
app_wifi_scan_sorted(aps, &n);
wifi_management_connect("MySSID", "pwd");
```

Do not block for long scans inside LVGL callbacks—run them from a worker or network page task.

---

[← Boot order](./03_boot.md) | [README](./README.md) | [MQTT (Bemfa) →](./05_mqtt.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
