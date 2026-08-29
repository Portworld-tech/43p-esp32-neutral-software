#include "app_wifi_util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_netif.h"
#include "esp_wifi.h"
#include "wifi_management.h"

static int app_wifi_rssi_cmp(const void *a, const void *b)
{
    const wifi_ap_record_t *ra = a;
    const wifi_ap_record_t *rb = b;
    return (int)rb->rssi - (int)ra->rssi;
}

esp_err_t app_wifi_scan_sorted(wifi_ap_record_t *recs, uint16_t *inout_count)
{
    if (recs == NULL || inout_count == NULL || *inout_count == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = wifi_management_scan_blocking(recs, inout_count);
    if (err != ESP_OK) {
        return err;
    }
    qsort(recs, *inout_count, sizeof(wifi_ap_record_t), app_wifi_rssi_cmp);
    return ESP_OK;
}

bool app_wifi_format_status(char *buf, size_t len)
{
    if (buf == NULL || len == 0) {
        return false;
    }
    buf[0] = '\0';
    if (!wifi_management_is_connected()) {
        snprintf(buf, len, "disconnected");
        return false;
    }

    wifi_ap_record_t ap = {0};
    esp_err_t err = esp_wifi_sta_get_ap_info(&ap);

    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_ip_info_t ip = {0};
    char ip_str[16] = "-";
    if (netif && esp_netif_get_ip_info(netif, &ip) == ESP_OK) {
        snprintf(ip_str, sizeof(ip_str), IPSTR, IP2STR(&ip.ip));
    }

    if (err == ESP_OK) {
        snprintf(buf, len, "%s | %s | %d dBm", (char *)ap.ssid, ip_str, ap.rssi);
    } else {
        snprintf(buf, len, "? | %s", ip_str);
    }
    return true;
}
