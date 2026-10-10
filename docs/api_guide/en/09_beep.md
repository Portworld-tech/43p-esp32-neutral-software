# 9. Buzzer

```c
#include "app_beep.h"
/* low-level */
#include "withthewind_board_lvgl_init.h"  /* board_beep_set */
```

| API | Role |
|-----|------|
| `board_beep_set(on)` | Direct on/off |
| `app_beep_pulse(ms)` | One-shot pulse (`esp_timer`) |
| `app_beep_pulse_n(n, on, off)` | Multi-beep |
| `app_beep_click_if_enabled()` | Respects `hub_model` click-sound setting |

```c
app_beep_pulse(120);
app_beep_click_if_enabled();
```

Safe to call from UI callbacks; pulses do not depend on LVGL timers.

---

[← Backlight](./08_backlight.md) | [README](./README.md) | [GPIO_OUT / RS485 →](./10_gpio_rs485.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
