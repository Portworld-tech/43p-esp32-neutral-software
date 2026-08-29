#include "app_low_power.h"

#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

static const char *TAG = "app_lp";

esp_err_t app_low_power_init(void)
{
#if CONFIG_PM_ENABLE
    /* RGB 全屏刷新：关闭 automatic light sleep，仅用 DFS 在 max/min 间降频。 */
    esp_pm_config_t pm = {
        .max_freq_mhz = CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = 160,
        .light_sleep_enable = false,
    };
    esp_err_t err = esp_pm_configure(&pm);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_pm_configure: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "PM: DFS %d-%d MHz, light_sleep=0",
             pm.min_freq_mhz, pm.max_freq_mhz);
#else
    ESP_LOGI(TAG, "CONFIG_PM_ENABLE is off; skip esp_pm_configure");
#endif
    return ESP_OK;
}
