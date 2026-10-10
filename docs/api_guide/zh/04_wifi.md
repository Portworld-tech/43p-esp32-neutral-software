# 4. Wi-Fi

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

---

[← 启动顺序](./03_boot.md) | [目录](./README.md) | [MQTT（巴法云） →](./05_mqtt.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
