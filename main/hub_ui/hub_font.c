#include "hub_font.h"

#include "lv_font_source_han_14_cjk3500.h"
#include "lv_font_hub_cjk_extra.h"
#include "lv_font_source_han_screen1_time_100.h"

static lv_font_t s_hub_font;
static lv_font_t s_hub_extra;
#if LV_FONT_SIMSUN_16_CJK
static lv_font_t s_hub_simsun;
#endif
static bool s_inited;

void hub_font_init(void)
{
    if (s_inited) {
        return;
    }
    s_inited = true;
    /*
     * Chain: Source Han 3500 → hub extras (钟/止/…)
     *      → SimSun 16 CJK (SSID / rare glyphs) → Montserrat (LV_SYMBOL_*).
     */
#if LV_FONT_SIMSUN_16_CJK
    s_hub_simsun = lv_font_simsun_16_cjk;
    s_hub_simsun.fallback = LV_FONT_DEFAULT;
    s_hub_extra = lv_font_hub_cjk_extra;
    s_hub_extra.fallback = &s_hub_simsun;
#else
    s_hub_extra = lv_font_hub_cjk_extra;
    s_hub_extra.fallback = LV_FONT_DEFAULT;
#endif
    s_hub_font = lv_font_source_han_14_cjk3500;
    s_hub_font.fallback = &s_hub_extra;
}

const lv_font_t *hub_font(void)
{
    if (!s_inited) {
        hub_font_init();
    }
    return &s_hub_font;
}

const lv_font_t *hub_font_clock(void)
{
    return &lv_font_montserrat_28;
}

const lv_font_t *hub_font_clock_lg(void)
{
    return &lv_font_montserrat_48;
}

const lv_font_t *hub_font_clock_xl(void)
{
    return &lv_font_source_han_screen1_time_100;
}

static void dropdown_list_font_cb(lv_event_t *e)
{
    lv_obj_t *dd = lv_event_get_target(e);
    lv_obj_t *list = lv_dropdown_get_list(dd);
    if (!list) {
        return;
    }
    const lv_font_t *f = hub_font();
    lv_obj_set_style_text_font(list, f, LV_PART_MAIN);
    lv_obj_set_style_text_font(list, f, LV_PART_SELECTED);
}

void hub_style_dropdown(lv_obj_t *dd)
{
    if (!dd) {
        return;
    }
    const lv_font_t *f = hub_font();
    lv_obj_set_style_text_font(dd, f, LV_PART_MAIN);
    lv_obj_set_style_text_font(dd, f, LV_PART_SELECTED);
    lv_obj_set_style_text_font(dd, &lv_font_montserrat_14, LV_PART_INDICATOR);
    lv_obj_t *list = lv_dropdown_get_list(dd);
    if (list) {
        lv_obj_set_style_text_font(list, f, LV_PART_MAIN);
        lv_obj_set_style_text_font(list, f, LV_PART_SELECTED);
    }
    /* List is created on open — re-apply font then. */
    lv_obj_add_event_cb(dd, dropdown_list_font_cb, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(dd, dropdown_list_font_cb, LV_EVENT_CLICKED, NULL);
}
