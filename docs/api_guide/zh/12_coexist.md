# 12. 射频共存

```c
#include "app_coexist.h"
```

| API | 时机 |
|-----|------|
| `app_coexist_before/after_ble_scan` | BLE 扫描 |
| `app_coexist_before_ethernet_work` | 以太网重负载 |
| `app_coexist_before/after_rs485_burst` | RS485 突发（`app_rs485_write` 内已调用） |
| `app_coexist_heap_ok_for_eth` | DMA 堆门禁 |

---

[← Modbus RTU](./11_modbus.md) | [目录](./README.md) | [低功耗 →](./13_low_power.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
