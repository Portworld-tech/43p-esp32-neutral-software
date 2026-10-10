# 13. 低功耗

```c
#include "app_low_power.h"
app_low_power_init();  /* foundation 之后、显示之前 */
```

RGB 场景下默认不开 Light sleep；待机用 `UI_AMBIENT_*` 降背光。

---

[← 射频共存](./12_coexist.md) | [目录](./README.md) | [跨线程 GUI →](./14_gui_task.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
