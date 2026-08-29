#pragma once
/**
 * @file app_gpio_out.h
 * @brief Expander GPIO_OUT helper (RS485 DE / general digital out).
 */

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t app_gpio_out_set(bool high);

/**
 * Read GPIO_OUT level when expander handle is available.
 * @return ESP_ERR_NOT_SUPPORTED if expander missing.
 */
esp_err_t app_gpio_out_get(bool *high_out);

/** Alias: RS485 DE/RE — true = transmit, false = receive. */
static inline esp_err_t app_rs485_de_set(bool tx_enable)
{
    return app_gpio_out_set(tx_enable);
}

#ifdef __cplusplus
}
#endif
