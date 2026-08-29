#include "app_coexist.h"

#include "board_ethernet_ch390.h"
#include "esp_coexist.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"
#include "sdkconfig.h"

#if defined(CONFIG_APP_FEATURE_BLE) && CONFIG_APP_FEATURE_BLE
#include "bt_management.h"
#endif

#if defined(CONFIG_APP_FEATURE_MQTT) && CONFIG_APP_FEATURE_MQTT
#include "wifi_bemfa_client.h"
#endif

static const char *TAG = "app_coex";

#define COEXIST_ETH_MIN_DMA_FREE    1536u
#define COEXIST_ETH_MIN_DMA_LARGEST 1280u

bool app_coexist_heap_ok_for_eth(void)
{
    const size_t free_bytes = heap_caps_get_free_size(MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    const size_t largest = heap_caps_get_largest_free_block(MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    return free_bytes >= COEXIST_ETH_MIN_DMA_FREE && largest >= COEXIST_ETH_MIN_DMA_LARGEST;
}

void app_coexist_log_heap(const char *label)
{
    ESP_LOGI(TAG, "%s: dma free=%lu largest=%lu int=%lu",
             label ? label : "heap",
             (unsigned long)heap_caps_get_free_size(MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL),
             (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL),
             (unsigned long)esp_get_free_internal_heap_size());
}

void app_coexist_before_ble_scan(void)
{
    board_ethernet_ch390_suspend_for_coexist();
#if CONFIG_ESP_COEX_SW_COEXIST_ENABLE
    (void)esp_coex_preference_set(ESP_COEX_PREFER_BT);
#endif
    app_coexist_log_heap("BLE scan prep");
}

void app_coexist_after_ble_scan(void)
{
#if CONFIG_ESP_COEX_SW_COEXIST_ENABLE
    (void)esp_coex_preference_set(ESP_COEX_PREFER_BALANCE);
#endif
    board_ethernet_ch390_set_traffic_paused(false);
}

void app_coexist_before_ethernet_work(void)
{
#if defined(CONFIG_APP_FEATURE_BLE) && CONFIG_APP_FEATURE_BLE
    bt_management_ble_scan_abort_if_active();
#endif
#if defined(CONFIG_APP_FEATURE_MQTT) && CONFIG_APP_FEATURE_MQTT
    wifi_bemfa_client_stop();
#endif
    board_ethernet_ch390_set_traffic_paused(false);
#if CONFIG_ESP_COEX_SW_COEXIST_ENABLE
    (void)esp_coex_preference_set(ESP_COEX_PREFER_WIFI);
#endif
    app_coexist_log_heap("before ETH work");
}

void app_coexist_before_rs485_burst(void)
{
    board_ethernet_ch390_set_traffic_paused(true);
}

void app_coexist_after_rs485_burst(void)
{
    board_ethernet_ch390_set_traffic_paused(false);
}
