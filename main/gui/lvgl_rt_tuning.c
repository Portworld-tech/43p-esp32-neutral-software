#include "lvgl_rt_tuning.h"

#include "sdkconfig.h"

#include "driver/gptimer.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "lvgl.h"

/* LVGL8: lvgl_port_task_notify lives in priv_include (not public esp_lvgl_port.h). */
extern bool lvgl_port_task_notify(uint32_t value);

static const char *TAG = "lvgl_rt";

static gptimer_handle_t s_lv_tick_timer;
static bool s_hw_tick_running;

static bool app_radio_features_enabled(void)
{
#if (defined(CONFIG_APP_FEATURE_BLE) && CONFIG_APP_FEATURE_BLE) || \
    (defined(CONFIG_APP_FEATURE_MESH) && CONFIG_APP_FEATURE_MESH)
    return true;
#else
    return false;
#endif
}

int lvgl_rt_lvgl_task_priority(void)
{
    if (app_radio_features_enabled()) {
        return CONFIG_APP_LVGL_TASK_PRIORITY;
    }
    return CONFIG_APP_LVGL_TASK_PRIORITY_STANDALONE;
}

int lvgl_rt_gui_task_priority(void)
{
    const int lv = lvgl_rt_lvgl_task_priority();
    const int gui = lv - CONFIG_APP_GUI_TASK_PRIO_BELOW_LVGL;
    return (gui < 1) ? 1 : gui;
}

int lvgl_rt_ui_bg_task_priority(void)
{
    return CONFIG_APP_UI_BG_TASK_PRIORITY;
}

int lvgl_rt_gui_task_core(void)
{
    /* Keep gui_task off taskLVGL's core so UI posting never preempts refresh. */
    return (CONFIG_APP_LVGL_TASK_CORE == 1) ? 0 : 1;
}

void lvgl_rt_fill_port_cfg(lvgl_port_cfg_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    cfg->task_affinity = CONFIG_APP_LVGL_TASK_CORE;
    cfg->task_priority = lvgl_rt_lvgl_task_priority();
    cfg->timer_period_ms = 1;
    /* 跟手：限制休眠，避免触摸后等半秒才跑 lv_timer_handler */
    cfg->task_max_sleep_ms = CONFIG_APP_LVGL_TASK_MAX_SLEEP_MS;
    if (cfg->task_max_sleep_ms > 20) {
        cfg->task_max_sleep_ms = 20;
    }
    cfg->task_stack = 8192;
    /* Must be INTERNAL: SPIFFS/PNG decode calls esp_flash_read (cache off).
     * A PSRAM stack fails esp_task_stack_is_sane_cache_disabled() assert. */
    cfg->task_stack_caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT;

    /* 定时器任务绑核 0 时，LVGL 独占核 1 更利于动画流畅度 */
    ESP_LOGI(TAG, "LVGL port: prio=%d core=%d stack=%d caps=0x%lx max_sleep=%d hw_tick=%d radio=%d",
             cfg->task_priority, cfg->task_affinity, cfg->task_stack,
             (unsigned long)cfg->task_stack_caps, cfg->task_max_sleep_ms,
#if defined(CONFIG_APP_LVGL_HW_GPTIMER_TICK) && CONFIG_APP_LVGL_HW_GPTIMER_TICK
             1,
#else
             0,
#endif
             app_radio_features_enabled() ? 1 : 0);
}

static bool IRAM_ATTR lvgl_rt_tick_isr(gptimer_handle_t timer,
                                       const gptimer_alarm_event_data_t *edata,
                                       void *user_ctx)
{
    (void)timer;
    (void)edata;
    (void)user_ctx;
    lv_tick_inc(1);
    return false;
}

esp_err_t lvgl_rt_start_hw_tick_1ms(void)
{
#if !defined(CONFIG_APP_LVGL_HW_GPTIMER_TICK) || !CONFIG_APP_LVGL_HW_GPTIMER_TICK
    return ESP_OK;
#else
    if (s_hw_tick_running) {
        return ESP_OK;
    }

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    ESP_RETURN_ON_ERROR(gptimer_new_timer(&timer_config, &s_lv_tick_timer), TAG, "gptimer_new");

    gptimer_event_callbacks_t cbs = {
        .on_alarm = lvgl_rt_tick_isr,
    };
    ESP_RETURN_ON_ERROR(gptimer_register_event_callbacks(s_lv_tick_timer, &cbs, NULL), TAG, "gptimer_cb");

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_RETURN_ON_ERROR(gptimer_set_alarm_action(s_lv_tick_timer, &alarm_config), TAG, "gptimer_alarm");
    ESP_RETURN_ON_ERROR(gptimer_enable(s_lv_tick_timer), TAG, "gptimer_enable");
    ESP_RETURN_ON_ERROR(gptimer_start(s_lv_tick_timer), TAG, "gptimer_start");

    s_hw_tick_running = true;
    ESP_LOGI(TAG, "LV tick: GPTimer 1ms (independent of FreeRTOS tick)");
    return ESP_OK;
#endif
}

void lvgl_rt_notify_lvgl_task(void)
{
    (void)lvgl_port_task_notify(1);
}
