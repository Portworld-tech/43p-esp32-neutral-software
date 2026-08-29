#include "app_beep.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "hub_model.h"
#include "withthewind_board_lvgl_init.h"

static const char *TAG = "app_beep";

typedef struct {
    int remaining;
    uint32_t ms_on;
    uint32_t ms_off;
    bool pin_high;
} app_beep_seq_t;

static SemaphoreHandle_t s_lock;
static esp_timer_handle_t s_timer;
static app_beep_seq_t s_seq;

static void app_beep_lock_init(void)
{
    if (s_lock == NULL) {
        s_lock = xSemaphoreCreateMutex();
    }
}

static void app_beep_timer_cb(void *arg)
{
    (void)arg;
    if (s_lock != NULL && xSemaphoreTake(s_lock, 0) != pdTRUE) {
        return;
    }

    if (s_seq.pin_high) {
        (void)board_beep_set(0);
        s_seq.pin_high = false;
        s_seq.remaining--;
        if (s_seq.remaining > 0) {
            (void)esp_timer_start_once(s_timer, (uint64_t)s_seq.ms_off * 1000ULL);
        }
    } else if (s_seq.remaining > 0) {
        (void)board_beep_set(1);
        s_seq.pin_high = true;
        (void)esp_timer_start_once(s_timer, (uint64_t)s_seq.ms_on * 1000ULL);
    }

    if (s_lock != NULL) {
        xSemaphoreGive(s_lock);
    }
}

static esp_err_t app_beep_ensure_timer(void)
{
    app_beep_lock_init();
    if (s_timer != NULL) {
        return ESP_OK;
    }
    const esp_timer_create_args_t args = {
        .callback = app_beep_timer_cb,
        .name = "app_beep",
    };
    return esp_timer_create(&args, &s_timer);
}

esp_err_t app_beep_pulse(uint32_t ms_on)
{
    return app_beep_pulse_n(1, ms_on > 0 ? ms_on : 80, 0);
}

esp_err_t app_beep_pulse_n(int count, uint32_t ms_on, uint32_t ms_off)
{
    if (count <= 0) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = app_beep_ensure_timer();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "timer create: %s", esp_err_to_name(err));
        return err;
    }

    app_beep_lock_init();
    if (xSemaphoreTake(s_lock, pdMS_TO_TICKS(50)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    (void)esp_timer_stop(s_timer);
    (void)board_beep_set(0);

    s_seq.remaining = count;
    s_seq.ms_on = ms_on > 0 ? ms_on : 80;
    s_seq.ms_off = ms_off;
    s_seq.pin_high = true;
    (void)board_beep_set(1);
    err = esp_timer_start_once(s_timer, (uint64_t)s_seq.ms_on * 1000ULL);

    xSemaphoreGive(s_lock);
    return err;
}

void app_beep_click_if_enabled(void)
{
    hub_model_t *m = hub_model();
    if (m != NULL && m->settings.click_sound) {
        (void)app_beep_pulse(60);
    }
}
