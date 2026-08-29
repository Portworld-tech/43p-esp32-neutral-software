#include "app_modbus_rtu.h"

#include <string.h>

#include "app_rs485.h"
#include "esp_check.h"
#include "esp_log.h"

static const char *TAG = "app_modbus";

uint16_t app_modbus_crc16(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF;
    if (data == NULL) {
        return crc;
    }
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

static esp_err_t app_modbus_transact_raw(const uint8_t *req, size_t req_len,
                                         uint8_t *rsp, size_t rsp_cap, int *rsp_len,
                                         uint32_t timeout_ms)
{
    if (!app_rs485_is_ready()) {
        ESP_RETURN_ON_ERROR(app_rs485_init(NULL), TAG, "rs485 init");
    }
    return app_rs485_transact(req, req_len, rsp, rsp_cap, rsp_len, 0, timeout_ms ? timeout_ms : 700);
}

esp_err_t app_modbus_read_holding(uint8_t slave, uint16_t addr, uint16_t qty,
                                  uint16_t *out_regs, size_t out_n, uint32_t timeout_ms)
{
    if (qty == 0 || out_regs == NULL || out_n < qty) {
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t req[8];
    req[0] = slave;
    req[1] = 0x03;
    req[2] = (uint8_t)(addr >> 8);
    req[3] = (uint8_t)(addr & 0xFF);
    req[4] = (uint8_t)(qty >> 8);
    req[5] = (uint8_t)(qty & 0xFF);
    uint16_t crc = app_modbus_crc16(req, 6);
    req[6] = (uint8_t)(crc & 0xFF);
    req[7] = (uint8_t)(crc >> 8);

    uint8_t rsp[5 + 2 * 125];
    int rsp_len = 0;
    size_t expect = (size_t)(5 + 2 * qty);
    if (expect > sizeof(rsp)) {
        return ESP_ERR_INVALID_SIZE;
    }
    esp_err_t err = app_modbus_transact_raw(req, sizeof(req), rsp, expect, &rsp_len, timeout_ms);
    if (err != ESP_OK) {
        return err;
    }
    if (rsp_len < 5 || rsp[0] != slave || rsp[1] != 0x03) {
        return ESP_ERR_INVALID_RESPONSE;
    }
    uint8_t byte_count = rsp[2];
    if (byte_count != (uint8_t)(qty * 2) || rsp_len < (int)(3 + byte_count + 2)) {
        return ESP_ERR_INVALID_SIZE;
    }
    uint16_t got_crc = (uint16_t)rsp[3 + byte_count] | ((uint16_t)rsp[4 + byte_count] << 8);
    if (got_crc != app_modbus_crc16(rsp, (size_t)(3 + byte_count))) {
        return ESP_ERR_INVALID_CRC;
    }
    for (uint16_t i = 0; i < qty; i++) {
        out_regs[i] = ((uint16_t)rsp[3 + 2 * i] << 8) | rsp[4 + 2 * i];
    }
    return ESP_OK;
}

esp_err_t app_modbus_write_single(uint8_t slave, uint16_t addr, uint16_t value, uint32_t timeout_ms)
{
    uint8_t req[8];
    req[0] = slave;
    req[1] = 0x06;
    req[2] = (uint8_t)(addr >> 8);
    req[3] = (uint8_t)(addr & 0xFF);
    req[4] = (uint8_t)(value >> 8);
    req[5] = (uint8_t)(value & 0xFF);
    uint16_t crc = app_modbus_crc16(req, 6);
    req[6] = (uint8_t)(crc & 0xFF);
    req[7] = (uint8_t)(crc >> 8);

    uint8_t rsp[8];
    int rsp_len = 0;
    esp_err_t err = app_modbus_transact_raw(req, sizeof(req), rsp, sizeof(rsp), &rsp_len, timeout_ms);
    if (err != ESP_OK) {
        return err;
    }
    if (rsp_len < 8) {
        return ESP_ERR_INVALID_SIZE;
    }
    uint16_t got_crc = (uint16_t)rsp[6] | ((uint16_t)rsp[7] << 8);
    if (got_crc != app_modbus_crc16(rsp, 6)) {
        return ESP_ERR_INVALID_CRC;
    }
    if (memcmp(req, rsp, 6) != 0) {
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}
