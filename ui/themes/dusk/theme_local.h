/**
 * Dusk theme pack — private API (not shared across themes).
 */
#pragma once

#include "hub_ui.h"
#include "hub_theme.h"
#include "lvgl.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* events */
void go_cb(lv_event_t *e);
void scene_cb(lv_event_t *e);
void proto_cb(lv_event_t *e);
void room_cb(lv_event_t *e);
void arm_cb(lv_event_t *e);
void zone_cb(lv_event_t *e);
void toggle_dev_cb(lv_event_t *e);
void step_ac_cb(lv_event_t *e);
void step_curtain_cb(lv_event_t *e);
void strip_step_cb(lv_event_t *e);
void curtain_set_cb(lv_event_t *e);

/* helpers */
lv_obj_t *lbl(lv_obj_t *par, const char *txt, lv_color_t c, const lv_font_t *f);
lv_obj_t *btn_go(lv_obj_t *par, const char *txt, hub_route_t r, bool accent);
void quickbar(lv_obj_t *par);
void theme_card_chrome(lv_obj_t *obj, bool active);

/* pages */
void build_home(lv_obj_t *parent);
void build_room(lv_obj_t *parent);
void build_scenes(lv_obj_t *parent);
void build_energy(lv_obj_t *parent);
void build_gateway(lv_obj_t *parent);
void build_network(lv_obj_t *parent);
void build_points(lv_obj_t *parent);
void build_settings(lv_obj_t *parent);
void build_security(lv_obj_t *parent);
void build_schedule(lv_obj_t *parent);
void build_hvac(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif
