#include "gui_task.h"
#include "lvgl_rt_tuning.h"

#include <stdint.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "lvgl.h"

#include "esp_lvgl_port.h"
#include "esp_log.h"

static const char *TAG = "gui_task";
static uint32_t s_drop_lock_cnt = 0;
static uint32_t s_drop_queue_cnt = 0;

#define GUI_QUEUE_LEN           24
#define GUI_LOCK_TIMEOUT_MS     40
#define GUI_LOCK_RETRY_COUNT    3
#define GUI_LOCK_RETRY_DELAY_MS 5
#define GUI_TASK_STACK_WORDS    5632

static StackType_t s_gui_stack[GUI_TASK_STACK_WORDS];
static StaticTask_t s_gui_tcb;

typedef struct {
    gui_lvgl_cb_t cb;
    void *user_data;
    gui_lvgl_fail_free_fn_t fail_free;
} gui_job_t;

static QueueHandle_t s_gui_queue = NULL;
static TaskHandle_t s_gui_task_handle = NULL;
static gui_job_t s_notify_job;

static void gui_job_release(const gui_job_t *job)
{
    if (job == NULL || job->user_data == NULL) {
        return;
    }
    if (job->fail_free != NULL) {
        job->fail_free(job->user_data);
    }
}

static bool gui_deliver_job(const gui_job_t *job)
{
    if (job == NULL || job->cb == NULL) {
        return false;
    }

    for (int attempt = 0; attempt < GUI_LOCK_RETRY_COUNT; attempt++) {
        if (lvgl_port_lock(GUI_LOCK_TIMEOUT_MS)) {
            lv_res_t res = lv_async_call((lv_async_cb_t)job->cb, job->user_data);
            lvgl_port_unlock();
            if (res == LV_RES_OK) {
                lvgl_rt_notify_lvgl_task();
                return true;
            }
            gui_job_release(job);
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(GUI_LOCK_RETRY_DELAY_MS));
    }

    s_drop_lock_cnt++;
    if ((s_drop_lock_cnt % 16u) == 1u) {
        ESP_LOGW(TAG, "LVGL lock busy, dropped=%lu", (unsigned long)s_drop_lock_cnt);
    }
    gui_job_release(job);
    return false;
}

static void gui_task(void *arg)
{
    (void)arg;

    gui_job_t job;
    uint32_t notify_val = 0;

    while (1) {
        while (xQueueReceive(s_gui_queue, &job, 0) == pdTRUE) {
            (void)gui_deliver_job(&job);
        }

        if (xTaskNotifyWait(0, ULONG_MAX, &notify_val, pdMS_TO_TICKS(50)) == pdPASS) {
            if (notify_val != 0 && notify_val != 1) {
                gui_job_t *njob = (gui_job_t *)(uintptr_t)notify_val;
                (void)gui_deliver_job(njob);
            }
            continue;
        }

        if (xQueueReceive(s_gui_queue, &job, portMAX_DELAY) == pdTRUE) {
            (void)gui_deliver_job(&job);
        }
    }
}

void gui_task_init(void)
{
    if (s_gui_queue != NULL) {
        return;
    }

    s_gui_queue = xQueueCreate(GUI_QUEUE_LEN, sizeof(gui_job_t));
    if (s_gui_queue == NULL) {
        ESP_LOGE(TAG, "xQueueCreate failed");
        return;
    }

    const int prio = lvgl_rt_gui_task_priority();
    const int core = lvgl_rt_gui_task_core();
    s_gui_task_handle = xTaskCreateStaticPinnedToCore(gui_task, "gui_task", GUI_TASK_STACK_WORDS, NULL, prio,
                                                      s_gui_stack, &s_gui_tcb, core);
    if (s_gui_task_handle == NULL) {
        ESP_LOGE(TAG, "xTaskCreatePinnedToCore failed");
        vQueueDelete(s_gui_queue);
        s_gui_queue = NULL;
        s_gui_task_handle = NULL;
        return;
    }

    ESP_LOGI(TAG, "started prio=%d core=%d", prio, core);
}

bool gui_task_notify_lvgl(gui_lvgl_cb_t cb, void *user_data)
{
    if (s_gui_task_handle == NULL || cb == NULL) {
        return false;
    }

    s_notify_job.cb = cb;
    s_notify_job.user_data = user_data;
    s_notify_job.fail_free = NULL;
    (void)xTaskNotify(s_gui_task_handle, (uint32_t)(uintptr_t)&s_notify_job, eSetValueWithOverwrite);
    return true;
}

bool gui_task_post_lvgl_ex(gui_lvgl_cb_t cb, void *user_data, gui_lvgl_fail_free_fn_t fail_free)
{
    if (s_gui_queue == NULL || cb == NULL) {
        return false;
    }

    gui_job_t job = {
        .cb = cb,
        .user_data = user_data,
        .fail_free = fail_free,
    };

    if (xQueueSend(s_gui_queue, &job, pdMS_TO_TICKS(50)) == pdTRUE) {
        if (s_gui_task_handle != NULL) {
            (void)xTaskNotify(s_gui_task_handle, 1, eNoAction);
        }
        return true;
    }

    s_drop_queue_cnt++;
    if ((s_drop_queue_cnt % 16u) == 1u) {
        ESP_LOGW(TAG, "gui queue full, dropped=%lu", (unsigned long)s_drop_queue_cnt);
    }
    gui_job_release(&job);
    return false;
}

bool gui_task_post_lvgl(gui_lvgl_cb_t cb, void *user_data)
{
    gui_lvgl_fail_free_fn_t ff = (user_data != NULL) ? free : NULL;
    return gui_task_post_lvgl_ex(cb, user_data, ff);
}
