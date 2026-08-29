#pragma once
/**
 * @file app_coexist.h
 * @brief Wi-Fi / BLE / Ethernet coexistence helpers (ported from testbench frame_coexist).
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool app_coexist_heap_ok_for_eth(void);
void app_coexist_log_heap(const char *tag);

/** Call before bt_management_ble_scan_start(). */
void app_coexist_before_ble_scan(void);
/** Call when scan finishes / aborts. */
void app_coexist_after_ble_scan(void);

/** Prefer Wi-Fi/ETH path: abort BLE scan, unpause CH390 SPI. */
void app_coexist_before_ethernet_work(void);

/** Pause CH390 SPI during RS485 bursts (lowers DMA pressure). */
void app_coexist_before_rs485_burst(void);
void app_coexist_after_rs485_burst(void);

#ifdef __cplusplus
}
#endif
