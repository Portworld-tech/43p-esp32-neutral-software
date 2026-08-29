#pragma once
/**
 * @file app_wifi_util.h
 * @brief Wi-Fi scan/status helpers for secondary development (no LVGL).
 */

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Blocking scan + sort by RSSI descending. @p inout_count capacity in / count out. */
esp_err_t app_wifi_scan_sorted(wifi_ap_record_t *recs, uint16_t *inout_count);

/** Format "SSID | IP | RSSI" into @p buf. Returns false if not connected. */
bool app_wifi_format_status(char *buf, size_t len);

#ifdef __cplusplus
}
#endif
