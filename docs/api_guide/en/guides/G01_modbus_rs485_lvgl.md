# G01. Walkthrough: Modbus + RS485 + LVGL device control

> **Goal:** Use the touch UI on this SDK to control Modbus slaves on RS485 (lights, curtains, relays, …).  
> **Audience:** You can already `idf.py flash`, but are unsure where button-press code should go.

---

## 1. Data flow (understand before coding)

```text
┌──────────────┐     ① mutate model   ┌────────────┐
│ LVGL button  │ ───────────────────► │ hub_model  │
└──────┬───────┘                      └─────┬──────┘
       │ ② enqueue (non-blocking)           │ ③ hub_ui_refresh()
       ▼                                    ▼
┌──────────────┐                      ┌────────────┐
│ FreeRTOS q   │                      │  UI paint  │
└──────┬───────┘                      └────────────┘
       │ ④ worker dequeues
       ▼
┌──────────────┐  ⑤ DE+UART    ┌────────────┐
│ app_modbus_* │ ────────────► │ RS485 slave│
└──────┬───────┘               └────────────┘
       │ ⑥ success / fail
       ▼
  app_beep / update_hub_health / (optional) MQTT sync
```

**Critical:** steps ①② on the LVGL thread are “mutate model + enqueue” only; step ⑤ must run in a dedicated task.

---

## 2. Hardware and Kconfig

### 2.1 Default pins (WithTheWind-validated)

| Signal | Default |
|--------|---------|
| UART | UART0 |
| TX / RX | GPIO43 / GPIO44 |
| DE/RE | Expander GPIO_OUT (high = transmit) |
| Baud | 115200 8N1 |

Confirm with your hardware vendor before production; override via `app_rs485_config_t`.

### 2.2 Console conflict

If `CONFIG_ESP_CONSOLE_UART_NUM=0`, UART0 fights RS485. Choose one:

1. menuconfig → console = **USB Serial/JTAG** (recommended)  
2. Or set `app_rs485_config_t` to `UART_NUM_1` + other TX/RX pins  

### 2.3 menuconfig

```
Secondary-dev peripherals
  [*] APP_ENABLE_RS485
  [ ] APP_RS485_AUTO_INIT     ← keep off until UART conflict is resolved
```

---

## 3. Implementation steps

### Step A — Prove the bus without UI

After display init in `app_main`, temporarily:

```c
#include "app_api.h"

app_rs485_init(NULL);
uint16_t reg = 0;
esp_err_t e = app_modbus_read_holding(0x01, 0x0000, 1, &reg, 1, 800);
ESP_LOGI("demo", "modbus read: %s val=%u", esp_err_to_name(e), reg);
```

Only wire UI after this succeeds. Failures: DE polarity, A/B wiring, slave address, baud, console conflict.

### Step B — Intent queue (recommended files)

Copy:

- `main/app/examples/app_bus_bridge_example.c` → `main/app/app_bus_bridge.c`  
- Matching header → `app_bus_bridge.h`  
- Add the `.c` to `MAIN_SRCS` in `main/CMakeLists.txt`  

Job shape:

```c
typedef enum {
    BUS_CMD_WRITE_HOLDING = 1,
    BUS_CMD_READ_HOLDING,
} bus_cmd_t;

typedef struct {
    bus_cmd_t cmd;
    uint8_t slave;
    uint16_t addr;
    uint16_t value;   /* write */
    int room, slot;   /* for UI feedback */
} bus_job_t;
```

Worker loop:

```c
for (;;) {
    bus_job_t job;
    if (xQueueReceive(q, &job, portMAX_DELAY) != pdTRUE) continue;

    esp_err_t err = ESP_FAIL;
    if (job.cmd == BUS_CMD_WRITE_HOLDING) {
        err = app_modbus_write_single(job.slave, job.addr, job.value, 800);
    }
    app_rs485_update_hub_health(err == ESP_OK, err == ESP_OK ? 95 : 20, true);
    if (err == ESP_OK) {
        app_beep_pulse(60);
    } else {
        hub_model_toast("Bus write failed");
        gui_task_post_lvgl(refresh_cb, NULL);
    }
}
```

### Step C — LVGL / theme callbacks only enqueue

In `theme_local.c` or a room-page button:

```c
#include "hub_model.h"
#include "hub_ui.h"
#include "app_bus_bridge.h"
#include "app_beep.h"

static void on_light_toggle(lv_event_t *e)
{
    (void)e;
    const int room = 0, slot = 0;
    hub_model_toggle_widget(room, slot);
    hub_ui_refresh();
    app_beep_click_if_enabled();

    hub_widget_t *w = hub_model_widget_by_slot(room, slot);
    if (w == NULL) return;

    /* Convention: slave 1, reg 0 = on/off 0/1 */
    app_bus_bridge_post_write(1, 0x0000, w->on ? 1 : 0);
}
```

**Do not** call `app_modbus_write_single` here.

### Step D — Optional poll-back

Every ~2 s in a worker:

```c
uint16_t v = 0;
if (app_modbus_read_holding(1, 0, 1, &v, 1, 500) == ESP_OK) {
    hub_model_set_widget_level(0, 0, v ? 100 : 0);
    gui_task_post_lvgl(refresh_cb, NULL);
}
```

Keeps the screen aligned if someone toggles the field switch manually.

### Step E — Optional MQTT fusion

After a successful local write:

```c
wifi_bemfa_client_publish_status_u8(item_id, value, true);
wifi_bemfa_client_schedule_sync();
```

Cloud-originated commands must call the **same** `app_bus_bridge_post_write` so phone / panel / bus stay consistent.

---

## 4. Example register map (replace with your device manual)

| Hub widget | Slave | FC | Addr | Meaning |
|------------|-------|----|------|---------|
| Living light ONOFF | 1 | 06 | 0x0000 | 0=off 1=on |
| Dimmer | 1 | 06 | 0x0001 | 0–100 |
| Curtain | 2 | 06 | 0x0010 | 0–100% |

Keep a static `widget → (slave, addr)` table inside `app_bus_bridge.c` so themes do not hard-code magic numbers.

---

## 5. Acceptance checklist

- [ ] `read_holding` succeeds with no UI  
- [ ] Touch toggle produces a visible write on scope / slave  
- [ ] Touch stays smooth (bus I/O off the LVGL thread)  
- [ ] Unplugging the bus shows toast / lower protocol health  
- [ ] (Optional) Phone MQTT matches panel state  

---

## 6. API quick reference

| Need | API |
|------|-----|
| Init bus | `app_rs485_init` |
| Write register | `app_modbus_write_single` |
| Read register | `app_modbus_read_holding` |
| Panel device state | `hub_model_toggle_widget` / `set_widget_level` |
| Refresh UI | `hub_ui_refresh` or `gui_task_post_lvgl` |
| Beep feedback | `app_beep_pulse` / `click_if_enabled` |
| Protocol slot | `app_rs485_update_hub_health` |

Details: [RS485](../10_gpio_rs485.md) · [Modbus](../11_modbus.md) · [Hub](../15_hub.md) · [gui_task](../14_gui_task.md)

Example sources: `main/app/examples/app_bus_bridge_example.c`
