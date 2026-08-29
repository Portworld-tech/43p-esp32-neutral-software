#pragma once
/**
 * @file app_rs485.h
 * @brief Half-duplex RS485 transport for secondary development.
 *
 * Defaults match WithTheWind bring-up (UART0 GPIO43/44 @ 115200, DE via expander GPIO_OUT).
 * Confirm pinout with your hardware vendor before production use.
 *
 * Never call blocking read/write from the LVGL task — use a worker or short timeouts.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver/uart.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uart_port_t uart_num; /**< default UART_NUM_0 */
    int tx_gpio;          /**< default 43 */
    int rx_gpio;          /**< default 44 */
    int baud;             /**< default 115200 */
    bool use_expander_de; /**< default true — DE via app_gpio_out / GPIO_OUT */
    uint32_t de_setup_ms; /**< default 5 */
    uint32_t tx_quiet_ms; /**< post-TX turnaround base, default 12 */
} app_rs485_config_t;

/** Fill @p out with board defaults. */
void app_rs485_get_default_config(app_rs485_config_t *out);

/**
 * Install UART + optional DE. Idempotent.
 * @param cfg NULL → board defaults.
 */
esp_err_t app_rs485_init(const app_rs485_config_t *cfg);

esp_err_t app_rs485_deinit(void);
bool app_rs485_is_ready(void);

/** Half-duplex write: DE high → TX → wait done → DE low. */
esp_err_t app_rs485_write(const uint8_t *data, size_t len, uint32_t timeout_ms);

/** Read up to @p cap bytes within @p wait_ms. Returns bytes read (>=0) or negative esp_err. */
int app_rs485_read(uint8_t *buf, size_t cap, uint32_t wait_ms);

/**
 * Write then collect response.
 * @param turnaround_ms extra delay after TX before RX window (0 = auto from length).
 */
esp_err_t app_rs485_transact(const uint8_t *tx, size_t tx_len,
                             uint8_t *rx, size_t rx_cap, int *rx_len,
                             uint32_t turnaround_ms, uint32_t rx_wait_ms);

/**
 * Update Hub protocol health slot (id "rs485" or fallback index 2) and optionally refresh UI.
 * Safe from non-LVGL tasks when @p post_ui_refresh is true (uses gui_task).
 */
void app_rs485_update_hub_health(bool link_ok, int health, bool post_ui_refresh);

#ifdef __cplusplus
}
#endif
