#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Background worker for blocking I/O (NVS, AHT20) — keeps taskLVGL responsive during switches. */
void ui_bg_task_init(void);

/** Queue temperature NVS write; returns false if queue full (caller may retry later). */
bool ui_bg_task_post_save_temps(int screen3_temp, int screen5_temp);

#if defined(CONFIG_AHT20_ENABLE) && CONFIG_AHT20_ENABLE
/** Last polled indoor temperature (°C rounded), or 999999 if unknown. Thread-safe read. */
int ui_bg_task_get_indoor_temp_cached(void);
#endif

#ifdef __cplusplus
}
#endif
