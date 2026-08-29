/** Sand — home (ThemeHubSand parity) */
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
    /* Sand paper: soft vertical wash only (FE uses 165deg CSS gradient) */
    lv_obj_set_style_bg_color(root, p->bg_deep, 0);
    lv_obj_set_style_bg_grad_color(root, p->bg_base, 0);
    lv_obj_set_style_bg_grad_dir(root, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_top(root, 24, 0);
    lv_obj_set_style_pad_bottom(root, 18, 0);
    lv_obj_set_style_pad_hor(root, 20, 0);
    lv_obj_set_style_pad_row(root, 8, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    /* Hero: eyebrow + clock + climate/scene | power orb */
    lv_obj_t *top = lv_obj_create(root);
    lv_obj_remove_style_all(top);
    lv_obj_set_width(top, LV_PCT(100));
    lv_obj_set_height(top, 118);
    lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
    lv_obj_set_style_pad_hor(top, 4, 0);

    lv_obj_t *left = lv_obj_create(top);
    lv_obj_remove_style_all(left);
    lv_obj_set_flex_grow(left, 1);
    lv_obj_set_height(left, LV_PCT(100));
    lv_obj_set_flex_flow(left, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(left, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(left, 4, 0);

    lv_obj_t *eye = lbl(left, "SAND HUB", p->accent, hub_font());
    lv_obj_set_style_text_letter_space(eye, 2, 0);

    hub_clock_label(left, p->t1, hub_font_clock_lg());

    char clim[72];
    snprintf(clim, sizeof(clim), hub_tr("室内 %.1f° · RH %d%% · %s", "Indoor %.1f° · RH %d%% · %s"),
             (double)m->indoor_c, m->rh, hub_model_scene_label(m->active_scene));
    lbl(left, clim, p->t3, hub_font());

    lv_obj_t *pwr = lv_btn_create(top);
    lv_obj_remove_style_all(pwr);
    hub_apply_card(pwr, false);
    theme_card_chrome(pwr, false);
    lv_obj_set_size(pwr, 72, 72);
    lv_obj_set_style_radius(pwr, 20, 0);
    lv_obj_set_style_pad_all(pwr, 0, 0);
    lv_obj_add_event_cb(pwr, go_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)HUB_ROUTE_ENERGY);
    hub_ico_add(pwr, HUB_ICO_GAUGE, p->accent, 22);
    lv_obj_align(lv_obj_get_child(pwr, 0), LV_ALIGN_TOP_MID, 0, 12);
    char pb[20];
    snprintf(pb, sizeof(pb), "%.1fkW", (double)m->power_kw);
    lv_obj_t *pv = lv_label_create(pwr);
    lv_label_set_text(pv, pb);
    hub_style_label(pv, p->t1, hub_font());
    lv_obj_align(pv, LV_ALIGN_BOTTOM_MID, 0, -10);

    /* Room list — 3 rows with 44×44 tinted badge (FE ThemeHubSand) */
    const char *subs[] = {
        hub_tr("空调 · 窗帘 · 灯", "AC · Curtain · Lights"),
        hub_tr("地暖 · 灯光", "Heat · Lights"),
        hub_tr("插座 · 水阀", "Outlet · Valve"),
    };
    lv_obj_t *list = lv_obj_create(root);
    lv_obj_remove_style_all(list);
    lv_obj_set_width(list, LV_PCT(100));
    lv_obj_set_flex_grow(list, 1);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 8, 0);
    lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < 3; i++) {
        bool on = hub_model_room_active(i);
        lv_obj_t *row = lv_btn_create(list);
        lv_obj_remove_style_all(row);
        hub_apply_card(row, false);
        theme_card_chrome(row, on);
        lv_obj_set_width(row, LV_PCT(100));
        lv_obj_set_height(row, 68);
        lv_obj_set_style_pad_hor(row, 16, 0);
        lv_obj_set_style_pad_ver(row, 12, 0);
        lv_obj_set_style_shadow_width(row, 12, 0);
        lv_obj_set_style_shadow_opa(row, LV_OPA_10, 0);
        lv_obj_set_style_shadow_ofs_y(row, 4, 0);
        lv_obj_set_style_shadow_color(row, p->t1, 0);
        lv_obj_add_event_cb(row, room_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);

        lv_obj_t *badge = lv_obj_create(row);
        lv_obj_remove_style_all(badge);
        lv_obj_set_size(badge, 44, 44);
        lv_obj_set_style_radius(badge, 14, 0);
        lv_obj_set_style_bg_opa(badge, LV_OPA_COVER, 0);
        if (on) {
            /* accent @ ~14% — approximate color-mix */
            lv_color_t tint = lv_color_mix(p->accent, p->bg_card, LV_OPA_20);
            lv_obj_set_style_bg_color(badge, tint, 0);
        } else {
            lv_obj_set_style_bg_color(badge, p->bg_card2, 0);
        }
        lv_obj_align(badge, LV_ALIGN_LEFT_MID, 0, 0);
        hub_ico_add(badge, HUB_ICO_HOME, on ? p->accent : p->t3, 22);
        lv_obj_center(lv_obj_get_child(badge, 0));

        lv_obj_t *n = lv_label_create(row);
        lv_label_set_text(n, hub_model_room_name(i));
        hub_style_label(n, p->t1, hub_font());
        lv_obj_align(n, LV_ALIGN_LEFT_MID, 58, -10);

        lv_obj_t *d = lv_label_create(row);
        lv_label_set_text(d, subs[i]);
        hub_style_label(d, p->t3, hub_font());
        lv_obj_align(d, LV_ALIGN_LEFT_MID, 58, 12);

        lv_obj_t *st = lv_label_create(row);
        lv_label_set_text(st, on ? hub_tr("运行", "Active") : hub_tr("待机", "Idle"));
        hub_style_label(st, on ? p->on : p->t4, hub_font());
        lv_obj_align(st, LV_ALIGN_RIGHT_MID, 0, 0);
    }

    quickbar(root);

    /* Scene CTAs — h=48 r=14 like FE sandBtn */
    lv_obj_t *cta = lv_obj_create(root);
    lv_obj_remove_style_all(cta);
    lv_obj_set_width(cta, LV_PCT(100));
    lv_obj_set_height(cta, 48);
    lv_obj_set_flex_flow(cta, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(cta, 10, 0);
    lv_obj_set_style_pad_top(cta, 2, 0);

    lv_obj_t *h = lv_btn_create(cta);
    lv_obj_remove_style_all(h);
    hub_apply_card(h, false);
    theme_card_chrome(h, false);
    lv_obj_set_style_radius(h, 14, 0);
    lv_obj_set_flex_grow(h, 1);
    lv_obj_set_height(h, LV_PCT(100));
    lv_obj_add_event_cb(h, scene_cb, LV_EVENT_CLICKED, (void *)"home");
    lv_obj_t *hl = lv_label_create(h);
    lv_label_set_text(hl, hub_model_scene_label("home"));
    hub_style_label(hl, p->t1, hub_font());
    lv_obj_center(hl);

    lv_obj_t *a = lv_btn_create(cta);
    lv_obj_remove_style_all(a);
    hub_apply_card(a, true);
    theme_card_chrome(a, true);
    lv_obj_set_style_bg_color(a, p->accent, 0);
    lv_obj_set_style_radius(a, 14, 0);
    lv_obj_set_style_border_width(a, 0, 0);
    lv_obj_set_flex_grow(a, 1);
    lv_obj_set_height(a, LV_PCT(100));
    lv_obj_add_event_cb(a, scene_cb, LV_EVENT_CLICKED, (void *)"away");
    lv_obj_t *al = lv_label_create(a);
    lv_label_set_text(al, hub_model_scene_label("away"));
    hub_style_label(al, p->ink_on, hub_font());
    lv_obj_center(al);
}
