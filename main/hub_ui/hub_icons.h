#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Hub icons — Smart-LVGL nt-style white PNG masks on SPIFFS
 * (spiffs_image/icons/nt/), recolored via lv_img.
 * Source: lvgl-front/lvgl-icons.jsx → tools/gen_hub_icons.py
 */
typedef enum {
    HUB_ICO_HOME = 0,
    HUB_ICO_BULB,
    HUB_ICO_CURTAIN,
    HUB_ICO_SNOW,
    HUB_ICO_SHIELD,
    HUB_ICO_GAUGE,
    HUB_ICO_LAYERS,
    HUB_ICO_WIFI,
    HUB_ICO_WIFI_OFF,
    HUB_ICO_COG,
    HUB_ICO_GRID,
    HUB_ICO_CLOCK,
    HUB_ICO_POWER,
    HUB_ICO_MOON,
    HUB_ICO_MOON_SLEEP,
    HUB_ICO_AWAY,
    HUB_ICO_BUS,
    HUB_ICO_DROP,
    HUB_ICO_HEAT,
    HUB_ICO_PLUS,
    HUB_ICO_MINUS,
    HUB_ICO_UP,
    HUB_ICO_DOWN,
    HUB_ICO_STOP,
    HUB_ICO_BACK,
    HUB_ICO_LEFT,
    HUB_ICO_RIGHT,
    HUB_ICO_CHEVRON,
    HUB_ICO_LINK,
    HUB_ICO_BRIGHT,
    HUB_ICO_INFO,
    HUB_ICO_LOCK,
    HUB_ICO_PLUG,
    HUB_ICO_FAN,
    HUB_ICO_SHUTTER,
    HUB_ICO_COUNT
} hub_ico_t;

/** Mask PNG base name (no path / extension), or NULL if unknown. */
const char *hub_ico_name(hub_ico_t ico);

/**
 * Decode all hub icons from SPIFFS into PSRAM once.
 * Call after SPIFFS mount / before first hub_ico_add.
 */
void hub_ico_cache_init(void);

/** Legacy Montserrat symbol (fallback only). */
const char *hub_ico_sym(hub_ico_t ico);

lv_obj_t *hub_ico_label(lv_obj_t *parent, hub_ico_t ico, lv_color_t color);

/**
 * Create icon sized to `size` pixels (96px mask zoomed).
 * Prefers SPIFFS PNG; falls back to symbol label if load fails.
 */
lv_obj_t *hub_ico_add(lv_obj_t *parent, hub_ico_t ico, lv_color_t color, lv_coord_t size);

/**
 * FE badge: plate + centered icon (SettingRow / SceneBtn).
 * Returns the badge container; icon is its first child.
 */
lv_obj_t *hub_ico_badge(lv_obj_t *parent, hub_ico_t ico, lv_color_t color, lv_coord_t badge_sz,
                        lv_coord_t ico_sz);

lv_obj_t *hub_clock_label(lv_obj_t *parent, lv_color_t color, const lv_font_t *font);

void hub_theme_wash(lv_obj_t *parent, lv_color_t accent, lv_opa_t opa);
void hub_hud_bevel(lv_obj_t *card, lv_color_t tick_color);
lv_obj_t *hub_energy_ring(lv_obj_t *parent, lv_color_t accent, lv_color_t cool);

#ifdef __cplusplus
}
#endif
