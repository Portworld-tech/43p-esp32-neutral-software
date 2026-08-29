#pragma once

#include "esp_err.h"

/** Mount SPIFFS on "/spiffs" (partition label "storage"). Call before ui_init / LVGL loads PNG paths. */
esp_err_t app_spiffs_mount(void);
