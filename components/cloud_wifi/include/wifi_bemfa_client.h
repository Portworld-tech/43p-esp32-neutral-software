#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start Bemfa MQTT client (phone -> cloud -> device).
 *
 * This module will:
 *  - connect to MQTT broker
 *  - subscribe cmd topic
 *  - parse incoming command payload
 *  - publish status payload to status topic
 */
esp_err_t wifi_bemfa_client_start(void);

/** Stop Bemfa MQTT client (used when Wi-Fi is not ready). */
void wifi_bemfa_client_stop(void);

/** Publish one MQTT status line for item ack (any task). */
void wifi_bemfa_client_publish_status_u8(uint8_t item_id, uint8_t value, bool ok);

/** Coalesced full-state JSON snapshot (LVGL thread executes publish). */
void wifi_bemfa_client_schedule_sync(void);

#ifdef __cplusplus
}
#endif

