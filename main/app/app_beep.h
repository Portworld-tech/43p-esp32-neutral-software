#pragma once
/**
 * @file app_beep.h
 * @brief Buzzer helpers for secondary development (esp_timer, no LVGL dependency).
 */

#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Single pulse: on for @p ms_on then auto-off. */
esp_err_t app_beep_pulse(uint32_t ms_on);

/** Multi-beep sequence. Non-blocking; overlaps cancel previous sequence. */
esp_err_t app_beep_pulse_n(int count, uint32_t ms_on, uint32_t ms_off);

/** Beep once if hub_model()->settings.click_sound is enabled. */
void app_beep_click_if_enabled(void);

#ifdef __cplusplus
}
#endif
