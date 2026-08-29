#pragma once

#include "app_ui_theme_select.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start the compile-selected UI pack after LVGL display is ready.
 * Theme: APP_UI_THEME_ID in app_ui_theme_select.h
 */
void app_ui_start(void);

#ifdef __cplusplus
}
#endif
