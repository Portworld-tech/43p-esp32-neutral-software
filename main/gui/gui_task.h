#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*gui_lvgl_cb_t)(void *user_data);

/** Called when a queued job is dropped (queue full / LVGL lock timeout). */
typedef void (*gui_lvgl_fail_free_fn_t)(void *user_data);

void gui_task_init(void);

bool gui_task_post_lvgl(gui_lvgl_cb_t cb, void *user_data);

/**
 * @brief Post LVGL job with explicit failure cleanup for heap user_data.
 *
 * Use when user_data is not plain malloc (e.g. custom struct). Pass NULL fail_free
 * only when user_data is NULL or owned elsewhere.
 */
bool gui_task_post_lvgl_ex(gui_lvgl_cb_t cb, void *user_data, gui_lvgl_fail_free_fn_t fail_free);

/**
 * @brief Lightweight path: static job + FreeRTOS task notify (no queue copy).
 * Wakes taskLVGL via lvgl_rt_notify_lvgl_task() for faster handoff.
 */
bool gui_task_notify_lvgl(gui_lvgl_cb_t cb, void *user_data);

#ifdef __cplusplus
}
#endif
