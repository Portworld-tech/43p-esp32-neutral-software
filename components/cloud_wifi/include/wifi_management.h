#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** When false, background scan task only scans on explicit refresh (testbench). */
void wifi_management_set_background_scan(bool enable);

/* Phase 1 foundation: NVS + netif + default event loop */
esp_err_t wifi_management_foundation_init(void);

/** 尽早 esp_wifi_init，在 RGB bounce / BLE 占用 DMA 之前拿到静态 TX/RX。可重试。 */
esp_err_t wifi_management_driver_init(void);

/* Start Wi-Fi STA and background scan updater */
esp_err_t wifi_management_start(void);

/** True after IP_EVENT_STA_GOT_IP (SNTP / cloud may run). */
bool wifi_management_is_connected(void);

/** When false, saved credentials are loaded but esp_wifi_connect is not called on start. */
void wifi_management_set_boot_auto_connect(bool enable);

/* Request a one-shot AP scan and dropdown refresh (e.g. Wi-Fi settings screen).
 * Safe from GUI/LVGL context; wakes the scan task without blocking. */
void wifi_management_request_scan_refresh(void);

/* Connect to target AP in STA mode */
esp_err_t wifi_management_connect(const char *ssid, const char *password);

/** Stop reconnect loop and disconnect (testbench [断开] button). */
void wifi_management_disconnect_user(void);

/**
 * Blocking scan for testbench. Pauses reconnect and returns AP list.
 * Safe to call from LVGL button handler (blocks ~1–3 s).
 */
esp_err_t wifi_management_scan_blocking(wifi_ap_record_t *recs, uint16_t *inout_count);

/** WiFi STA 是否已成功启动（esp_wifi_start 完成）。 */
bool wifi_management_is_started(void);

/** 启动是否已因内存等原因永久失败（需重启）。 */
bool wifi_management_start_failed(void);

/** True while the Wi‑Fi scan is repainting the SSID dropdown (programmatic select). */
bool wifi_management_is_dropdown_refresh(void);

/** Apply deferred SSID list refresh after the dropdown popup closes (LV_EVENT_CANCEL). */
void wifi_management_dropdown_list_closed(void);

#ifdef __cplusplus
}
#endif

