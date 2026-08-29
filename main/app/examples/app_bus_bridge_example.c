/**
 * @file app_bus_bridge_example.c
 * @brief Copy to main/app/app_bus_bridge.c and add to MAIN_SRCS for Modbus+LVGL bridge.
 *
 * Pattern: LVGL/theme posts jobs → worker task talks Modbus on RS485.
 * See docs/api_guide/zh/guides/G01_modbus_rs485_lvgl.md
 */
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "app_beep.h"
#include "app_modbus_rtu.h"
#include "app_rs485.h"
#include "gui_task.h"
#include "hub_model.h"
#include "hub_ui.h"

static const char *TAG = "bus_bridge";

typedef enum {
    BUS_CMD_WRITE = 1,
} bus_cmd_t;

typedef struct {
    bus_cmd_t cmd;
    uint8_t slave;
    uint16_t addr;
    uint16_t value;
} bus_job_t;

static QueueHandle_t s_q;

static void refresh_cb(void *arg)
{
    (void)arg;
    hub_ui_refresh();
}

static void bus_worker(void *arg)
{
    (void)arg;
    bus_job_t job;
    for (;;) {
        if (xQueueReceive(s_q, &job, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (!app_rs485_is_ready()) {
            if (app_rs485_init(NULL) != ESP_OK) {
                ESP_LOGW(TAG, "rs485 init failed");
                continue;
            }
        }

        esp_err_t err = ESP_ERR_NOT_SUPPORTED;
        if (job.cmd == BUS_CMD_WRITE) {
            err = app_modbus_write_single(job.slave, job.addr, job.value, 800);
        }

        app_rs485_update_hub_health(err == ESP_OK, err == ESP_OK ? 95 : 15, true);
        if (err == ESP_OK) {
            app_beep_pulse(50);
        } else {
            ESP_LOGW(TAG, "modbus write fail: %s", esp_err_to_name(err));
            hub_model_toast("总线写入失败");
            (void)gui_task_post_lvgl(refresh_cb, NULL);
        }
    }
}

esp_err_t app_bus_bridge_start(void)
{
    if (s_q != NULL) {
        return ESP_OK;
    }
    s_q = xQueueCreate(8, sizeof(bus_job_t));
    if (s_q == NULL) {
        return ESP_ERR_NO_MEM;
    }
    BaseType_t ok = xTaskCreate(bus_worker, "bus_bridge", 4096, NULL, 5, NULL);
    return ok == pdPASS ? ESP_OK : ESP_FAIL;
}

bool app_bus_bridge_post_write(uint8_t slave, uint16_t addr, uint16_t value)
{
    if (s_q == NULL) {
        return false;
    }
    bus_job_t job = {
        .cmd = BUS_CMD_WRITE,
        .slave = slave,
        .addr = addr,
        .value = value,
    };
    return xQueueSend(s_q, &job, 0) == pdTRUE;
}

/*
 * Example theme callback (paste into theme_local / room page):
 *
 *   hub_model_toggle_widget(0, 0);
 *   hub_ui_refresh();
 *   hub_widget_t *w = hub_model_widget_by_slot(0, 0);
 *   if (w) {
 *       app_bus_bridge_post_write(1, 0x0000, w->on ? 1 : 0);
 *   }
 */
