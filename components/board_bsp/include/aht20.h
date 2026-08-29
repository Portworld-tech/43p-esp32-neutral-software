#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Init AHT20 on the shared TP_SCL/TP_SDA bus (idempotent). */
esp_err_t aht20_init(i2c_master_bus_handle_t bus);

/** Read calibrated temperature in °C. Returns ESP_ERR_INVALID_STATE if not init. */
esp_err_t aht20_read_temperature_c(float *temp_c);

#ifdef __cplusplus
}
#endif
