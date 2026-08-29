/** Forest — home */
#include "theme_local.h"
#include "hub_ui.h"
#include "hub_theme.h"
#include "hub_font.h"
#include "hub_model.h"
#include "hub_icons.h"
#include "hub_i18n.h"
#include "app_ui.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

void build_home(lv_obj_t *parent)
{
    const hub_palette_t *p = hub_palette();
    hub_model_t *m = hub_model();
    lv_obj_t *root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, 480, 480);
    hub_apply_screen_bg(root);
    hub_theme_wash(root, p->accent, LV_OPA_20);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(root, 14, 0);
    lv_obj_set_style_pad_row(root, 8, 0);
    lv_obj_set_flex_align(root, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *head = lv_obj_create(root);
    lv_obj_remove_style_all(head);
    lv_obj_set_width(head, LV_PCT(100));
    lv_obj_set_height(head, 36);
    lv_obj_set_flex_flow(head, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(head, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    hub_clock_label(head, p->t1, hub_font());
    lbl(head, "Forest", p->t1, hub_font());
    lv_obj_t *eco = btn_go(head, "Eco", HUB_ROUTE_ENERGY, true);
    lv_obj_set_size(eco, 56, 28);

    lv_obj_t *ring_c = hub_energy_ring(root, p->accent, p->cool);
    lv_obj_t *ring = lv_obj_get_parent(ring_c);
    lv_obj_add_event_cb(ring, go_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)HUB_ROUTE_ENERGY);
    lbl(ring_c, hub_tr("今日用电", "Today"), p->t3, hub_font());
    lv_obj_align(lv_obj_get_child(ring_c, 0), LV_ALIGN_CENTER, 0, -28);
    char kwh[16];
    snprintf(kwh, sizeof(kwh), "%.1f", (double)(12.0 + m->power_kw * 2));
    lv_obj_t *kv = lv_label_create(ring_c);
    lv_label_set_text(kv, kwh);
    hub_style_label(kv, p->t1, hub_font_clock());
    lv_obj_align(kv, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *ku = lv_label_create(ring_c);
    lv_label_set_text(ku, "kWh");
    hub_style_label(ku, p->t3, hub_font());
    lv_obj_align(ku, LV_ALIGN_CENTER, 0, 28);

    lv_obj_t *stats = lv_obj_create(root);
    lv_obj_remove_style_all(stats);
    lv_obj_set_width(stats, LV_PCT(100));
    lv_obj_set_height(stats, 48);
    lv_obj_set_flex_flow(stats, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(stats, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    char ib[16], rb[16];
    snprintf(ib, sizeof(ib), "%.0f°", (double)m->indoor_c);
    snprintf(rb, sizeof(rb), "%d%%", m->rh);
    const char *vals[] = { ib, rb, hub_model_ok_count() == HUB_PROTO_COUNT ? "OK" : "CHK" };
    const char *labs[] = {
        hub_tr("室内", "Indoor"), hub_tr("湿度", "Humidity"), hub_tr("总线", "Bus"),
    };
    hub_route_t rts[] = { HUB_ROUTE_HVAC, HUB_ROUTE_HVAC, HUB_ROUTE_GATEWAY };
    for (int i = 0; i < 3; i++) {
        lv_obj_t *b = lv_btn_create(stats);
        lv_obj_remove_style_all(b);
        lv_obj_set_size(b, 90, 48);
        lv_obj_add_event_cb(b, go_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)rts[i]);
        lv_obj_t *v = lv_label_create(b);
        lv_label_set_text(v, vals[i]);
        hub_style_label(v, (i == 2 && hub_model_ok_count() == HUB_PROTO_COUNT) ? p->on : p->t1, hub_font());
        lv_obj_align(v, LV_ALIGN_TOP_MID, 0, 2);
        lv_obj_t *l = lv_label_create(b);
        lv_label_set_text(l, labs[i]);
        hub_style_label(l, p->t3, hub_font());
        lv_obj_align(l, LV_ALIGN_BOTTOM_MID, 0, -2);
    }

    lv_obj_t *scenes = lv_obj_create(root);
    lv_obj_remove_style_all(scenes);
    lv_obj_set_width(scenes, LV_PCT(100));
    lv_obj_set_flex_grow(scenes, 1);
    lv_obj_set_flex_flow(scenes, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_row(scenes, 8, 0);
    lv_obj_set_style_pad_column(scenes, 8, 0);
    const char *ids[] = {"home", "guest", "movie", "sleep", "eco", "alloff"};
    for (int i = 0; i < 6; i++) {
        bool on = strcmp(m->active_scene, ids[i]) == 0
                  || (strcmp(ids[i], "alloff") == 0 && strcmp(m->active_scene, "away") == 0);
        lv_obj_t *b = lv_btn_create(scenes);
        lv_obj_remove_style_all(b);
        hub_apply_card(b, on);
        theme_card_chrome(b, on);
        if (on) {
            lv_obj_set_style_bg_color(b, p->accent, 0);
        }
        lv_obj_set_style_radius(b, 999, 0);
        lv_obj_set_height(b, 36);
        lv_obj_set_style_pad_hor(b, 12, 0);
        lv_obj_add_event_cb(b, scene_cb, LV_EVENT_CLICKED, (void *)ids[i]);
        lv_obj_t *l = lv_label_create(b);
        lv_label_set_text(l, hub_model_scene_label(ids[i]));
        hub_style_label(l, on ? p->ink_on : p->t1, hub_font());
        lv_obj_center(l);
    }
    quickbar(root);
}
