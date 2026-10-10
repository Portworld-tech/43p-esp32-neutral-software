# 14. 跨线程 GUI（详细）

> LVGL 不是线程安全的。Wi-Fi 事件、Modbus 任务、MQTT 回调里**不能**直接 `lv_label_set_text`，必须投递到 LVGL 线程。

---

## 1. API

```c
#include "gui_task.h"

gui_task_init();   /* main.c 已在 UI 后调用 */

gui_task_post_lvgl(cb, user_data);
gui_task_post_lvgl_ex(cb, user_data, fail_free);  /* 队列满时释放堆 */
gui_task_notify_lvgl(cb, user_data);              /* 轻量路径 */
```

---

## 2. 标准写法

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
        /* post 失败时 fail_free 已处理；若返回 false 视实现再 free */
    }
}
```

改 Hub 整页时更简单：

```c
static void refresh_cb(void *arg) { (void)arg; hub_ui_refresh(); }
gui_task_post_lvgl(refresh_cb, NULL);
```

`app_rs485_update_hub_health(..., true)` 内部已走 `gui_task`。

---

## 3. 与总线开发的关系

```text
总线任务 ──成功/失败──► gui_task_post_lvgl ──► hub_ui_refresh / toast
LVGL 回调 ──只──► hub_model + queueSend（绝不 wait 总线）
```

详见 [G01](./guides/G01_modbus_rs485_lvgl.md)。
