#pragma once
/* Pair with app_bus_bridge_example.c — rename to app_bus_bridge.h when integrating. */

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t app_bus_bridge_start(void);
bool app_bus_bridge_post_write(uint8_t slave, uint16_t addr, uint16_t value);

#ifdef __cplusplus
}
#endif
