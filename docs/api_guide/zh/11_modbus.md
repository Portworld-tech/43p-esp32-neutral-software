# 11. Modbus RTU（详细）

> 在 `app_rs485` 之上的薄客户端：功能码 **03 读保持寄存器**、**06 写单寄存器**。够用多数灯控/继电器/模块；复杂设备可自扩 FC。

---

## 1. 依赖关系

```text
app_modbus_*  ──依赖──►  app_rs485_*  ──依赖──►  UART + DE(GPIO_OUT)
```

```c
#include "app_modbus_rtu.h"
#include "app_rs485.h"
```

首次调用前需 `app_rs485_init`（`read_holding` 内也会尝试 init）。

---

## 2. API

| 函数 | 功能码 | 说明 |
|------|--------|------|
| `app_modbus_crc16(data,len)` | — | 标准 Modbus CRC16 |
| `app_modbus_read_holding(slave,addr,qty,out,out_n,timeout_ms)` | 0x03 | 读保持寄存器 |
| `app_modbus_write_single(slave,addr,value,timeout_ms)` | 0x06 | 写单寄存器 |

### 读示例

```c
uint16_t regs[4];
esp_err_t err = app_modbus_read_holding(
    0x01,      /* 从站地址 */
    0x0000,    /* 起始寄存器 */
    2,         /* 数量 */
    regs, 4,
    700);
if (err == ESP_OK) {
    ESP_LOGI("mb", "r0=%u r1=%u", regs[0], regs[1]);
}
```

### 写示例

```c
/* 打开继电器：地址 0 写 1 */
esp_err_t err = app_modbus_write_single(0x01, 0x0000, 0x0001, 700);
```

探测帧（与测试台一致）：`01 03 00 00 00 01 84 0A`

---

## 3. 与 LVGL / Hub 的正确接法

错误：

```c
void btn_cb(lv_event_t *e) {
    app_modbus_write_single(...);  /* 阻塞触摸！ */
}
```

正确：见 [G01](./guides/G01_modbus_rs485_lvgl.md) — 回调只改 `hub_model` + 入队，任务里写 Modbus。

映射建议：

| Widget 类型 | 建议寄存器语义 |
|-------------|----------------|
| ONOFF | 0/1 |
| DIMMER / CURTAIN | 0–100 |
| CLIM 设定点 | 温度 ×10 等（按手册） |

---

## 4. 错误码

常见返回：`ESP_OK`、`ESP_ERR_TIMEOUT`、`ESP_ERR_INVALID_CRC`、`ESP_ERR_INVALID_RESPONSE`、`ESP_ERR_INVALID_SIZE`。  
失败时调用 `app_rs485_update_hub_health(false, …)` 让运维页可见。

---

## 5. 扩展

需要 FC05/0F/10 时：用 `app_modbus_crc16` + `app_rs485_transact` 自组帧，或引入 esp-modbus 仍走同一 UART（注意与本薄封装互斥）。

下一章：[射频共存](./12_coexist.md) · 实战：[G01](./guides/G01_modbus_rs485_lvgl.md)
