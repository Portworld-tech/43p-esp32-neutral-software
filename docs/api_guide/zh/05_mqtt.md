# 5. MQTT（巴法云）

```c
#include "wifi_bemfa_client.h"
```

| API | 作用 |
|-----|------|
| `wifi_bemfa_client_start/stop` | 启停（GOT_IP 时常自动 start） |
| `wifi_bemfa_client_publish_status_u8` | 单点应答 |
| `wifi_bemfa_client_schedule_sync` | 全量快照 |

需 `APP_FEATURE_MQTT=y`。本地改态后：`hub_model_*` → `hub_ui_refresh()` → `schedule_sync()`。

---

[← Wi-Fi](./04_wifi.md) | [目录](./README.md) | [蓝牙 BLE / Mesh →](./06_bt.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
