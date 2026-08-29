#pragma once
/**
 * Customer-safe IO expander API.
 *
 * Full LCD/CH390 pin maps stay inside board_bsp.a. Secondary development should
 * use these helpers (GPIO_OUT / DE / RTC INT) instead of hard-coding expander indices.
 */

#include <stdbool.h>

#include "esp_err.h"
#include "esp_io_expander.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Handle created during board_display_start (NULL if expander unavailable). */
esp_io_expander_handle_t board_io_expander_get(void);

/**
 * Drive NCA9555 GPIO_OUT (schematic P1.0).
 * On WithTheWind this line is shared as RS485 DE/RE (high = transmit).
 */
esp_err_t board_io_expander_gpio_out_set(bool high);

/** RTCIC_INT_L: true when interrupt asserted (active low). */
bool board_io_expander_rtc_int_active(void);

#ifdef __cplusplus
}
#endif
