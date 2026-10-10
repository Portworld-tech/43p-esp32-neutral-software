# 8. 背光

```c
#include "withthewind_board_lvgl_init.h"
#include "hub_device_ui.h"
```

`board_backlight_init` → 首帧后 `board_backlight_on/set(0..100)`。设置页：`hub_device_brightness_get/apply_effective`。

---

[← 以太网](./07_eth.md) | [目录](./README.md) | [蜂鸣器 →](./09_beep.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
