# 11. Modbus RTU (detailed)

> Thin client on top of `app_rs485`: function codes **03 read holding** and **06 write single register**. Enough for most lights / relays / modules; extend FC yourself for complex devices.

---

## 1. Dependencies

```text
app_modbus_*  ──depends──►  app_rs485_*  ──depends──►  UART + DE(GPIO_OUT)
```

```c
#include "app_modbus_rtu.h"
#include "app_rs485.h"
```

Call `app_rs485_init` before first use (`read_holding` may also attempt init).

---

## 2. API

| Function | FC | Description |
|----------|----|-------------|
| `app_modbus_crc16(data,len)` | — | Standard Modbus CRC16 |
| `app_modbus_read_holding(slave,addr,qty,out,out_n,timeout_ms)` | 0x03 | Read holding registers |
| `app_modbus_write_single(slave,addr,value,timeout_ms)` | 0x06 | Write single register |

### Read example

```c
uint16_t regs[4];
esp_err_t err = app_modbus_read_holding(
    0x01,      /* slave */
    0x0000,    /* start register */
    2,         /* quantity */
    regs, 4,
    700);
if (err == ESP_OK) {
    ESP_LOGI("mb", "r0=%u r1=%u", regs[0], regs[1]);
}
```

### Write example

```c
/* turn relay on: write 1 to address 0 */
esp_err_t err = app_modbus_write_single(0x01, 0x0000, 0x0001, 700);
```

Probe frame (same as testbench): `01 03 00 00 00 01 84 0A`

---

## 3. Correct LVGL / Hub wiring

Wrong:

```c
void btn_cb(lv_event_t *e) {
    app_modbus_write_single(...);  /* blocks touch! */
}
```

Right: see [G01](./guides/G01_modbus_rs485_lvgl.md) — callback only mutates `hub_model` + enqueues; Modbus runs in a worker.

Mapping tips:

| Widget type | Typical register meaning |
|-------------|--------------------------|
| ONOFF | 0/1 |
| DIMMER / CURTAIN | 0–100 |
| CLIM setpoint | temp ×10 etc. (per device manual) |

---

## 4. Error codes

Common returns: `ESP_OK`, `ESP_ERR_TIMEOUT`, `ESP_ERR_INVALID_CRC`, `ESP_ERR_INVALID_RESPONSE`, `ESP_ERR_INVALID_SIZE`.  
On failure call `app_rs485_update_hub_health(false, …)` so the ops page shows it.

---

## 5. Extending

For FC05/0F/10: build frames with `app_modbus_crc16` + `app_rs485_transact`, or bring in esp-modbus on the **same** UART (do not mix with this thin client concurrently).

Next: [RF coexistence](./12_coexist.md) · Hands-on: [G01](./guides/G01_modbus_rs485_lvgl.md)
