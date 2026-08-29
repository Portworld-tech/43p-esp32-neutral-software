#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Source Han SC 14 → hub CJK extra → SimSun (SSID) → Montserrat symbols. */
const lv_font_t *hub_font(void);

/** Large clock digits (Montserrat). */
const lv_font_t *hub_font_clock(void);
const lv_font_t *hub_font_clock_lg(void);
/** XL digits for Zen / minimal homes (~100px Source Han time subset). */
const lv_font_t *hub_font_clock_xl(void);

void hub_font_init(void);

/**
 * Apply hub CJK font to dropdown button + open list (MAIN/SELECTED).
 * Indicator stays Montserrat so the ▼ glyph remains visible.
 */
void hub_style_dropdown(lv_obj_t *dd);

#ifdef __cplusplus
}
#endif
