# 13. Low power

```c
#include "app_low_power.h"
app_low_power_init();  /* after foundation, before display */
```

On RGB panels, Light sleep is typically **not** enabled by default. For idle power saving, use `UI_AMBIENT_*` to dim the backlight instead of deep sleep.

See also: [Backlight](./08_backlight.md) · direction 6 in [G00](./guides/G00_directions.md).

---

[← RF coexistence](./12_coexist.md) | [README](./README.md) | [Cross-thread GUI →](./14_gui_task.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
