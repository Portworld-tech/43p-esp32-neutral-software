#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Init AHT20 on the shared TP_SCL/TP_SDA bus (idempotent). */
esp_err_t aht20_init(i2c_master_bus_handle_t bus);

/**
 * Trigger one measurement and fill temperature (°C) and relative humidity (%RH).
 * Either output pointer may be NULL if that value is not needed.
 */
esp_err_t aht20_read(float *temp_c, float *rh_pct);

/** Read calibrated temperature in °C. Returns ESP_ERR_INVALID_STATE if not init. */
esp_err_t aht20_read_temperature_c(float *temp_c);

/** Read relative humidity in %RH (0..100). Returns ESP_ERR_INVALID_STATE if not init. */
esp_err_t aht20_read_humidity_rh(float *rh_pct);

#ifdef __cplusplus
}
#endif
