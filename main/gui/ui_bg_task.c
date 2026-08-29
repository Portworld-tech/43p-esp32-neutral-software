#include "ui_bg_task.h"

#include "gui_task.h"
#include "lvgl_rt_tuning.h"

#include "sdkconfig.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "nvs.h"

#if defined(CONFIG_AHT20_ENABLE) && CONFIG_AHT20_ENABLE
#include "aht20.h"
#include "withthewind_board_lvgl_init.h"
#endif

static const char *TAG = "ui_bg";

#define UI_BG_QUEUE_LEN     8
#define UI_BG_TASK_STACK    4096
#define UI_BG_I2C_LOCK_MS   200
/* Indoor temp changes slowly; 2s polling only burned I2C when AHT20 missing. */
#define UI_BG_TEMP_POLL_MS           10000
#define UI_BG_AHT20_RETRY_BASE_MS    5000
#define UI_BG_AHT20_RETRY_MAX_MS     (30 * 60 * 1000)

#define UI_EXTRA_TEMP_NS     "ui_extra_temp"
#define UI_EXTRA_KEY_SCREEN3 "screen3_temp"
#define UI_EXTRA_KEY_SCREEN5 "screen5_temp"

typedef enum {
    UI_BG_JOB_SAVE_TEMPS,
} ui_bg_job_type_t;

typedef struct {
    ui_bg_job_type_t type;
    int arg0;
    int arg1;
} ui_bg_job_t;

static QueueHandle_t s_queue;
static SemaphoreHandle_t s_i2c_mux;
static uint32_t s_drop_cnt;

#if defined(CONFIG_AHT20_ENABLE) && CONFIG_AHT20_ENABLE
static bool s_aht20_ready;
static int s_indoor_cached = 999999;
static volatile int s_indoor_lvgl_pending = 999999;
static TickType_t s_aht20_next_action;
static uint8_t s_aht20_fail_streak;
static bool s_aht20_absent_logged;

static void ui_bg_indoor_lvgl_cb(void *user_data)
{
    (void)user_data;
    const int ti = s_indoor_lvgl_pending;
    extern void ui_runtime_indoor_apply_temp(int temp_c);
    ui_runtime_indoor_apply_temp(ti);
}

static bool ui_bg_i2c_lock(void)
{
    if (s_i2c_mux == NULL) {
        return false;
    }
    return xSemaphoreTake(s_i2c_mux, pdMS_TO_TICKS(UI_BG_I2C_LOCK_MS)) == pdTRUE;
}

static void ui_bg_i2c_unlock(void)
{
    if (s_i2c_mux != NULL) {
        (void)xSemaphoreGive(s_i2c_mux);
    }
}

static uint32_t ui_bg_aht20_retry_ms(uint8_t streak)
{
    /* 5s → 10s → 20s → … capped at 30min (many boards ship without AHT20). */
    uint8_t shift = streak;
    if (shift > 8) {
        shift = 8;
    }
    uint32_t ms = (uint32_t)UI_BG_AHT20_RETRY_BASE_MS << shift;
    if (ms > (uint32_t)UI_BG_AHT20_RETRY_MAX_MS) {
        ms = (uint32_t)UI_BG_AHT20_RETRY_MAX_MS;
    }
    return ms;
}

static void ui_bg_aht20_schedule(uint32_t delay_ms)
{
    s_aht20_next_action = xTaskGetTickCount() + pdMS_TO_TICKS(delay_ms);
}

static TickType_t ui_bg_aht20_wait_ticks(void)
{
    const TickType_t now = xTaskGetTickCount();
    if (now >= s_aht20_next_action) {
        return 0;
    }
    return s_aht20_next_action - now;
}

static void ui_bg_aht20_ensure_init(void)
{
    if (s_aht20_ready) {
        return;
    }
    i2c_master_bus_handle_t bus = board_i2c_get_handle();
    if (bus == NULL) {
        ui_bg_aht20_schedule(ui_bg_aht20_retry_ms(s_aht20_fail_streak));
        return;
    }
    if (!ui_bg_i2c_lock()) {
        ui_bg_aht20_schedule(UI_BG_AHT20_RETRY_BASE_MS);
        return;
    }
    esp_err_t err = aht20_init(bus);
    ui_bg_i2c_unlock();
    if (err == ESP_OK) {
        s_aht20_ready = true;
        s_aht20_fail_streak = 0;
        s_aht20_absent_logged = false;
        ui_bg_aht20_schedule(UI_BG_TEMP_POLL_MS);
        ESP_LOGI(TAG, "AHT20 ready (bg task)");
        return;
    }

    const uint32_t retry_ms = ui_bg_aht20_retry_ms(s_aht20_fail_streak);
    if (s_aht20_fail_streak < 255) {
        s_aht20_fail_streak++;
    }
    ui_bg_aht20_schedule(retry_ms);

    if (!s_aht20_absent_logged) {
        s_aht20_absent_logged = true;
        ESP_LOGW(TAG, "AHT20 unavailable (%s); retry in %lus (backoff)",
                 esp_err_to_name(err), (unsigned long)(retry_ms / 1000u));
    } else {
        ESP_LOGD(TAG, "AHT20 still unavailable (%s); next try %lus",
                 esp_err_to_name(err), (unsigned long)(retry_ms / 1000u));
    }
}

static void ui_bg_aht20_poll(void)
{
    if (!s_aht20_ready) {
        ui_bg_aht20_ensure_init();
        return;
    }
    if (!ui_bg_i2c_lock()) {
        ui_bg_aht20_schedule(UI_BG_AHT20_RETRY_BASE_MS);
        return;
    }
    float c = 0.0f;
    esp_err_t err = aht20_read_temperature_c(&c);
    ui_bg_i2c_unlock();
    ui_bg_aht20_schedule(UI_BG_TEMP_POLL_MS);
    if (err != ESP_OK) {
        return;
    }

    const int ti = (int)(c >= 0.0f ? (c + 0.5f) : (c - 0.5f));
    if (ti == s_indoor_cached) {
        return;
    }
    s_indoor_cached = ti;
    s_indoor_lvgl_pending = ti;
    (void)gui_task_notify_lvgl(ui_bg_indoor_lvgl_cb, NULL);
}
#endif /* CONFIG_AHT20_ENABLE */

static void ui_bg_save_temps_now(int s3, int s5)
{
    nvs_handle_t h;
    if (nvs_open(UI_EXTRA_TEMP_NS, NVS_READWRITE, &h) != ESP_OK) {
        return;
    }
    (void)nvs_set_u32(h, UI_EXTRA_KEY_SCREEN3, (uint32_t)s3);
    (void)nvs_set_u32(h, UI_EXTRA_KEY_SCREEN5, (uint32_t)s5);
    (void)nvs_commit(h);
    nvs_close(h);
    ESP_LOGD(TAG, "temps NVS committed (bg)");
}

static bool ui_bg_post_job(ui_bg_job_type_t type, int arg0, int arg1)
{
    if (s_queue == NULL) {
        return false;
    }
    const ui_bg_job_t job = {
        .type = type,
        .arg0 = arg0,
        .arg1 = arg1,
    };
    if (xQueueSend(s_queue, &job, 0) == pdTRUE) {
        return true;
    }
    s_drop_cnt++;
    if ((s_drop_cnt % 8u) == 1u) {
        ESP_LOGW(TAG, "bg queue full, dropped=%lu", (unsigned long)s_drop_cnt);
    }
    return false;
}

static void ui_bg_task(void *arg)
{
    (void)arg;
    ui_bg_job_t job;

    while (1) {
#if defined(CONFIG_AHT20_ENABLE) && CONFIG_AHT20_ENABLE
        while (1) {
            if (xQueueReceive(s_queue, &job, ui_bg_aht20_wait_ticks()) == pdTRUE) {
                break;
            }
            ui_bg_aht20_poll();
        }
#else
        if (xQueueReceive(s_queue, &job, portMAX_DELAY) != pdTRUE) {
            continue;
        }
#endif

        switch (job.type) {
        case UI_BG_JOB_SAVE_TEMPS:
            ui_bg_save_temps_now(job.arg0, job.arg1);
            break;
        default:
            break;
        }
    }
}

void ui_bg_task_init(void)
{
    if (s_queue != NULL) {
        return;
    }

    s_i2c_mux = xSemaphoreCreateMutex();
    if (s_i2c_mux == NULL) {
        ESP_LOGE(TAG, "i2c mutex create failed");
        return;
    }

    s_queue = xQueueCreate(UI_BG_QUEUE_LEN, sizeof(ui_bg_job_t));
    if (s_queue == NULL) {
        ESP_LOGE(TAG, "queue create failed");
        vSemaphoreDelete(s_i2c_mux);
        s_i2c_mux = NULL;
        return;
    }

    const int prio = lvgl_rt_ui_bg_task_priority();
    const BaseType_t res = xTaskCreatePinnedToCore(ui_bg_task, "ui_bg", UI_BG_TASK_STACK, NULL,
                                                   prio, NULL, 1);
    if (res != pdPASS) {
        ESP_LOGE(TAG, "task create failed");
        vQueueDelete(s_queue);
        s_queue = NULL;
        vSemaphoreDelete(s_i2c_mux);
        s_i2c_mux = NULL;
        return;
    }

    ESP_LOGI(TAG, "background I/O task started prio=%d", prio);
}

bool ui_bg_task_post_save_temps(int screen3_temp, int screen5_temp)
{
    if (!ui_bg_post_job(UI_BG_JOB_SAVE_TEMPS, screen3_temp, screen5_temp)) {
        ui_bg_save_temps_now(screen3_temp, screen5_temp);
        return false;
    }
    return true;
}

#if defined(CONFIG_AHT20_ENABLE) && CONFIG_AHT20_ENABLE
int ui_bg_task_get_indoor_temp_cached(void)
{
    return s_indoor_cached;
}
#endif
