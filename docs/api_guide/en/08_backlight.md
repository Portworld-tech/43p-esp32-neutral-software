# 8. Backlight

```c
#include "withthewind_board_lvgl_init.h"
#include "hub_device_ui.h"
```

Call `board_backlight_init`, then after the first frame `board_backlight_on` / `board_backlight_set(0..100)`.  
Settings page helpers: `hub_device_brightness_get` / `hub_device_brightness_apply_effective`.

Idle dimming is usually driven by `UI_AMBIENT_*` (see [Low power](./13_low_power.md)).

---

[← Ethernet](./07_eth.md) | [README](./README.md) | [Buzzer →](./09_beep.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
