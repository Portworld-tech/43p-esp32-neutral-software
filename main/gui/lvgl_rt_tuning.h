#pragma once

#include "esp_err.h"
#include "esp_lvgl_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Fill lvgl_port_cfg: task priority / 1ms tick period / Core1 affinity. */
void lvgl_rt_fill_port_cfg(lvgl_port_cfg_t *cfg);

/** Start GPTimer 1ms ISR → lv_tick_inc(1); skips esp_lvgl_port esp_timer when Kconfig enabled. */
esp_err_t lvgl_rt_start_hw_tick_1ms(void);

int lvgl_rt_lvgl_task_priority(void);
int lvgl_rt_gui_task_priority(void);
int lvgl_rt_ui_bg_task_priority(void);
int lvgl_rt_gui_task_core(void);

/** Wake taskLVGL promptly (e.g. after bg I/O posts an LVGL job). */
void lvgl_rt_notify_lvgl_task(void);

#ifdef __cplusplus
}
#endif
