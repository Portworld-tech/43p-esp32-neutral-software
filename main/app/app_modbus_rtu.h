#pragma once
/**
 * @file app_modbus_rtu.h
 * @brief Thin Modbus RTU client on top of app_rs485 (secondary development).
 */

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

uint16_t app_modbus_crc16(const uint8_t *data, size_t len);

/**
 * FC 0x03 Read Holding Registers.
 * @param out_regs buffer for @p qty registers (big-endian decoded to host uint16).
 */
esp_err_t app_modbus_read_holding(uint8_t slave, uint16_t addr, uint16_t qty,
                                  uint16_t *out_regs, size_t out_n, uint32_t timeout_ms);

/** FC 0x06 Write Single Register. */
esp_err_t app_modbus_write_single(uint8_t slave, uint16_t addr, uint16_t value, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
