#include "app_spiffs.h"

#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "spiffs";

esp_err_t app_spiffs_mount(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 16,
        .format_if_mount_failed = true,
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_vfs_spiffs_register failed: %s", esp_err_to_name(ret));
        return ret;
    }

    size_t total = 0;
    size_t used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "partition=%s used=%u total=%u", conf.partition_label, (unsigned)used, (unsigned)total);
    }

    return ESP_OK;
}
