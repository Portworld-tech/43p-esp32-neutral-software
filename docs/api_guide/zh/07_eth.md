# 7. 以太网

```c
#include "board_ethernet_ch390.h"
#include "app_coexist.h"
```

| API | 作用 |
|-----|------|
| `board_ethernet_ch390_init/try_start` | 初始化 / 启动 |
| `get_ip` / `link_up` / `get_link_info` | 状态 |
| `set_traffic_paused` | 暂停 SPI（RS485 突发时由 `app_coexist_*_rs485_burst` 调用） |

重负载前：`app_coexist_before_ethernet_work()`。

---

[← 蓝牙 BLE / Mesh](./06_bt.md) | [目录](./README.md) | [背光 →](./08_backlight.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
