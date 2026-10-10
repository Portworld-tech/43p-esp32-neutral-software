# 14. Cross-thread GUI (detailed)

> LVGL is **not** thread-safe. Wi-Fi events, Modbus workers, and MQTT callbacks must **not** call `lv_label_set_text` directly—post work to the LVGL thread.

---

## 1. API

```c
#include "gui_task.h"

gui_task_init();   /* already called after UI in main.c */

gui_task_post_lvgl(cb, user_data);
gui_task_post_lvgl_ex(cb, user_data, fail_free);  /* free heap if queue full */
gui_task_notify_lvgl(cb, user_data);              /* lightweight path */
```

---

## 2. Standard pattern

```c
typedef struct {
    lv_obj_t *label;
    char text[48];
} ui_msg_t;

static void apply_cb(void *arg)
{
    ui_msg_t *m = arg;
    lv_label_set_text(m->label, m->text);
    free(m);
}

void from_bus_task(const char *status)
{
    ui_msg_t *m = malloc(sizeof(*m));
    m->label = s_status_label;
    snprintf(m->text, sizeof(m->text), "%s", status);
    if (!gui_task_post_lvgl_ex(apply_cb, m, free)) {
        /* fail_free already ran on queue-full; check impl if false */
    }
}
```

Refreshing the whole Hub page is simpler:

```c
static void refresh_cb(void *arg) { (void)arg; hub_ui_refresh(); }
gui_task_post_lvgl(refresh_cb, NULL);
```

`app_rs485_update_hub_health(..., true)` already goes through `gui_task`.

---

## 3. Relation to bus work

```text
Bus task ──success/fail──► gui_task_post_lvgl ──► hub_ui_refresh / toast
LVGL callback ──only──► hub_model + QueueSend (never wait on the bus)
```

Details: [G01](./guides/G01_modbus_rs485_lvgl.md).
