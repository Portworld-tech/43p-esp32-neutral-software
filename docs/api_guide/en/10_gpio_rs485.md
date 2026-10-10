# 10. GPIO_OUT / RS485 (detailed)

> Half-duplex RS485 is the transport layer for field devices in this SDK. Modbus and custom protocols sit on top of it.

---

## 1. Role

| API group | Role |
|-----------|------|
| `app_gpio_out_*` | Expander GPIO_OUT; used as RS485 DE/RE on this board |
| `app_rs485_*` | UART half-duplex: DE high → TX → wait done → DE low → RX |

Headers:

```c
#include "app_rs485.h"
#include "app_gpio_out.h"
#include "board_io_expander.h"   /* safe subset */
```

---

## 2. Hardware defaults

| Item | Default | Notes |
|------|---------|-------|
| UART | `UART_NUM_0` | Pair with USB-JTAG console |
| TX / RX | GPIO 43 / 44 | WithTheWind-validated |
| DE | `app_gpio_out_set(true)` to transmit | High = TX, low = RX |
| Baud | 115200 8N1 | Overridable via config |

```c
app_rs485_config_t cfg;
app_rs485_get_default_config(&cfg);
cfg.uart_num = UART_NUM_1;   /* example: avoid console UART */
cfg.tx_gpio = /* confirm with hardware vendor */;
cfg.rx_gpio = /* ... */;
cfg.baud = 9600;
app_rs485_init(&cfg);
```

---

## 3. API reference

| Function | Description |
|----------|-------------|
| `app_rs485_init(cfg)` | Install driver; `cfg==NULL` uses defaults; idempotent |
| `app_rs485_is_ready()` | Whether init succeeded |
| `app_rs485_write(data,len,timeout)` | Half-duplex write; pauses ETH SPI internally |
| `app_rs485_read(buf,cap,wait_ms)` | Timed read; returns byte count or negative error |
| `app_rs485_transact(...)` | Write then read reply |
| `app_rs485_update_hub_health(ok,health,refresh)` | Update ops-page protocol slot |
| `app_rs485_deinit()` | Release UART |

DE timing (inside write):

```text
DE=1 → delay de_setup_ms → uart_write → wait_tx_done → delay turnaround → DE=0
```

---

## 4. Threading (required)

| Thread | Allowed |
|--------|---------|
| LVGL / button callbacks | Only `xQueueSend` intents — **no** long `read` / `transact` |
| Your bus worker task | `write` / `read` / `modbus_*` |
| UI feedback | `gui_task_post_lvgl` or `update_hub_health(..., true)` |

Full light-control flow → [G01](./guides/G01_modbus_rs485_lvgl.md)

---

## 5. Kconfig

| Option | Default | Meaning |
|--------|---------|---------|
| `APP_ENABLE_RS485` | y | Compile this module |
| `APP_RS485_AUTO_INIT` | n | `app_main` auto-`init` (enable only after UART conflict is resolved) |

---

## 6. Troubleshooting

| Symptom | Check |
|---------|-------|
| TX but no reply | DE polarity, A/B wiring, termination, slave address |
| Init garbles log / crashes | UART0 vs console conflict → JTAG or another UART |
| Touch stutters | Bus I/O ran on the LVGL thread |

Next: [Modbus RTU](./11_modbus.md)
