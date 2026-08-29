#pragma once

#include "esp_err.h"

/**
 * 应用级低功耗初始化（DFS / PM 框架）。
 * 须在 NVS、网络栈基础初始化之后尽早调用；详见 docs/LOW_POWER_DESIGN.md。
 */
esp_err_t app_low_power_init(void);
