// BT-CTRL protocol (shared by BLE GATT / SPP / Mesh vendor)
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BTCTRL_MAGIC0 0xB7
#define BTCTRL_MAGIC1 0xC1
#define BTCTRL_VERSION 1

typedef enum {
    BTCTRL_MSG_HELLO      = 0x01,
    BTCTRL_MSG_HELLO_RSP  = 0x81,

    BTCTRL_MSG_AUTH_START = 0x02,
    BTCTRL_MSG_AUTH_PROOF = 0x03,

    BTCTRL_MSG_GET_STATUS = 0x10,
    BTCTRL_MSG_SET_STATE  = 0x11,
    BTCTRL_MSG_GET_MESH_STATUS = 0x12,
    BTCTRL_MSG_DATA       = 0x13,
    BTCTRL_MSG_GET_LINK   = 0x14,

    BTCTRL_MSG_ACK        = 0x82,

    BTCTRL_MSG_MESH_SEND  = 0x20,
    BTCTRL_MSG_MESH_STATUS= 0x21,
    BTCTRL_MSG_DATA_IND   = 0x23,
    BTCTRL_MSG_LINK_STATUS= 0x24,

    BTCTRL_MSG_ERROR_RSP  = 0x7F,
} btctrl_msg_type_t;

typedef struct __attribute__((packed)) {
    uint8_t magic[2];
    uint8_t ver;
    uint8_t msg_type;
    uint16_t seq;
    uint16_t flags;
    uint16_t len;
    uint16_t crc16;
    uint32_t reserved;
} btctrl_hdr_t;

typedef struct {
    btctrl_hdr_t hdr;
    const uint8_t *body;
} btctrl_frame_view_t;

typedef esp_err_t (*btctrl_tx_fn_t)(const uint8_t *data, size_t len, void *user);

uint16_t btctrl_crc16_update(uint16_t crc, const uint8_t *data, size_t len);
uint16_t btctrl_crc16(const uint8_t *data, size_t len);

/**
 * @brief Basic framing validation (magic/ver/len/crc). Does not parse body.
 */
esp_err_t btctrl_parse_view(const uint8_t *buf, size_t buf_len, btctrl_frame_view_t *out);

/**
 * @brief Encode header + body into out buffer.
 */
esp_err_t btctrl_build(uint8_t msg_type, uint16_t seq, uint16_t flags,
                       const uint8_t *body, uint16_t body_len,
                       uint8_t *out, size_t out_cap, size_t *out_len);

#ifdef __cplusplus
}
#endif

