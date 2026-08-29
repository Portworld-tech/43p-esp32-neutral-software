#include "app_health.h"

#include "sdkconfig.h"

#if defined(CONFIG_APP_HEALTH_MONITOR) && CONFIG_APP_HEALTH_MONITOR

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "app_health";

#define HEALTH_PERIOD_US        (60ULL * 1000000ULL)
#define HEALTH_PSRAM_WARN_BYTES   (128U * 1024U)
#define HEALTH_INT_WARN_BYTES     (16U * 1024U)
#define HEALTH_LOW_STREAK_LIMIT   5U

static esp_timer_handle_t s_health_timer;
static uint8_t s_psram_low_streak;
static uint8_t s_int_low_streak;

static void app_health_timer_cb(void *arg)
{
    (void)arg;

    const size_t int_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    const size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    const size_t int_min = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    const size_t psram_min = heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    ESP_LOGI(TAG, "heap int free=%u min=%u | psram free=%u min=%u",
             (unsigned)int_free, (unsigned)int_min,
             (unsigned)psram_free, (unsigned)psram_min);

    if (psram_free < HEALTH_PSRAM_WARN_BYTES || psram_min < HEALTH_PSRAM_WARN_BYTES) {
        s_psram_low_streak++;
        ESP_LOGW(TAG, "PSRAM low streak=%u (free=%u min=%u)",
                 (unsigned)s_psram_low_streak, (unsigned)psram_free, (unsigned)psram_min);
    } else {
        s_psram_low_streak = 0;
    }

    if (int_free < HEALTH_INT_WARN_BYTES || int_min < HEALTH_INT_WARN_BYTES) {
        s_int_low_streak++;
        ESP_LOGW(TAG, "internal heap low streak=%u (free=%u min=%u)",
                 (unsigned)s_int_low_streak, (unsigned)int_free, (unsigned)int_min);
    } else {
        s_int_low_streak = 0;
    }

#if CONFIG_APP_HEALTH_MONITOR_RESET_ON_OOM
    if (s_psram_low_streak >= HEALTH_LOW_STREAK_LIMIT || s_int_low_streak >= HEALTH_LOW_STREAK_LIMIT) {
        ESP_LOGE(TAG, "sustained low heap — restarting");
        esp_restart();
    }
#endif
}

esp_err_t app_health_monitor_start(void)
{
    if (s_health_timer != NULL) {
        return ESP_OK;
    }

    const esp_timer_create_args_t args = {
        .callback = app_health_timer_cb,
        .name = "app_health",
        .dispatch_method = ESP_TIMER_TASK,
        .skip_unhandled_events = true,
    };
    esp_err_t err = esp_timer_create(&args, &s_health_timer);
    if (err != ESP_OK) {
        return err;
    }
    return esp_timer_start_periodic(s_health_timer, HEALTH_PERIOD_US);
}

#else /* !CONFIG_APP_HEALTH_MONITOR */

esp_err_t app_health_monitor_start(void)
{
    return ESP_OK;
}

#endif
