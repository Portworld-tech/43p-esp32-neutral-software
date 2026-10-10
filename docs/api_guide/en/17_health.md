# 17. Health monitor

```c
#include "app_health.h"
app_health_monitor_start();
```

Optional heap / system health task (`APP_HEALTH_MONITOR`). Protocol health on the ops page is updated separately via `app_rs485_update_hub_health` and `hub_model()->protos[]`.

---

[← Icons](./16_icons.md) | [README](./README.md) | [Kconfig →](./18_kconfig.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
