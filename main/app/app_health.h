#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Periodic heap / stack health logging for long-run diagnostics. */
esp_err_t app_health_monitor_start(void);

#ifdef __cplusplus
}
#endif
