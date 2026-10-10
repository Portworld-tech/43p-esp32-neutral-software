# 12. RF coexistence

```c
#include "app_coexist.h"
```

| API | When |
|-----|------|
| `app_coexist_before/after_ble_scan` | Around BLE scans |
| `app_coexist_before_ethernet_work` | Before heavy Ethernet traffic |
| `app_coexist_before/after_rs485_burst` | RS485 bursts (`app_rs485_write` already calls these) |
| `app_coexist_heap_ok_for_eth` | DMA-heap gate for ETH |

CH390 shares SPI with other activity; pausing SPI during RS485 / coordinating around BLE scans prevents link drops and garbled bus frames.

---

[← Modbus RTU](./11_modbus.md) | [README](./README.md) | [Low power →](./13_low_power.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
