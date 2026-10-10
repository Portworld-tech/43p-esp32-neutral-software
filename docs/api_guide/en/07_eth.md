# 7. Ethernet

```c
#include "board_ethernet_ch390.h"
#include "app_coexist.h"
```

| API | Role |
|-----|------|
| `board_ethernet_ch390_init/try_start` | Init / start |
| `get_ip` / `link_up` / `get_link_info` | Status |
| `set_traffic_paused` | Pause SPI (used by `app_coexist_*_rs485_burst` during RS485 bursts) |

Before heavy ETH work: `app_coexist_before_ethernet_work()`.

Gateway pattern: [G04](./guides/G04_eth_rs485_gateway.md).

---

[← Bluetooth](./06_bt.md) | [README](./README.md) | [Backlight →](./08_backlight.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
