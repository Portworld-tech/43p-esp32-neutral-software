#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** BLE 星型：单 Central GATT 链路状态（本机为 Peripheral） */
typedef struct {
    bool connected;
    bool notify_enabled;
    uint16_t mtu;
    uint8_t peer_bda[6];
} bt_gatt_star_link_t;

#define BT_GATT_STAR_LOG_MAX   12
#define BT_GATT_STAR_LOG_BYTES 96

typedef struct {
    bool from_central;
    uint16_t len;
    uint8_t data[BT_GATT_STAR_LOG_BYTES];
    uint32_t tick_ms;
} bt_gatt_star_log_entry_t;

void bt_gatt_star_set_link(bool connected, bool notify_enabled, uint16_t mtu, const uint8_t peer_bda[6]);
void bt_gatt_star_get_link(bt_gatt_star_link_t *out);

void bt_gatt_star_log_rx(const uint8_t *data, size_t len);
void bt_gatt_star_log_tx(const uint8_t *data, size_t len);
unsigned bt_gatt_star_log_count(void);
bool bt_gatt_star_log_get(unsigned idx_from_newest, bt_gatt_star_log_entry_t *out);
void bt_gatt_star_log_clear(void);

/** 将最近日志格式化为多行文本（供 LVGL 显示） */
void bt_gatt_star_format_log(char *buf, size_t buf_len, unsigned max_lines);

#ifdef __cplusplus
}
#endif
