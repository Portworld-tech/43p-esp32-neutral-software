# 9. 蜂鸣器

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

---

[← 背光](./08_backlight.md) | [目录](./README.md) | [GPIO_OUT / RS485 →](./10_gpio_rs485.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
