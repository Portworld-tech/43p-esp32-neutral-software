#pragma once
/* Public board API for customer SDK. Pin maps / expander nets are not included. */

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_lvgl_port.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t board_display_start_with_lvgl_cfg(const lvgl_port_cfg_t *lvgl_cfg);
i2c_master_bus_handle_t board_i2c_get_handle(void);
esp_err_t board_backlight_init(void);
esp_err_t board_backlight_set(int brightness_percent);
int board_backlight_quantize_percent(int brightness_percent);
int board_backlight_clamp_percent(int brightness_percent);
esp_err_t board_backlight_on(void);
esp_err_t board_backlight_off(void);
esp_err_t board_beep_set(int on);
void board_display_present_fb(const void *rendered_fb);
void board_display_present_frame(void);

#ifdef __cplusplus
}
#endif
