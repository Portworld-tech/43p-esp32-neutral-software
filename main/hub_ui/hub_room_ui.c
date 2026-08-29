#include "hub_room_ui.h"

#include "hub_ui.h"
#include "hub_theme.h"
#include "hub_font.h"
#include "hub_model.h"
#include "hub_icons.h"
#include "hub_i18n.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define PAGE_SIZE 6

typedef struct {
    int room;
    int slot;
    int delta;
} w_ud_t;

static w_ud_t s_ud_pool[32];
static int s_ud_n;

static w_ud_t *ud_alloc(int room, int slot, int delta)
{
    if (s_ud_n >= 32) {
        s_ud_n = 0;
    }
    w_ud_t *u = &s_ud_pool[s_ud_n++];
    u->room = room;
    u->slot = slot;
    u->delta = delta;
    return u;
}

static void refresh_cb(lv_event_t *e)
{
    (void)e;
    hub_ui_refresh();
}

static void go_edit_cb(lv_event_t *e)
{
    (void)e;
    hub_ui_go(HUB_ROUTE_ROOM_EDIT);
}

static void go_room_cb(lv_event_t *e)
{
    int idx = (int)(uintptr_t)lv_event_get_user_data(e);
    hub_model_set_room(idx);
    hub_ui_go(HUB_ROUTE_ROOM);
}

static void go_home_cb(lv_event_t *e)
{
    (void)e;
    hub_ui_go(HUB_ROUTE_HOME);
}

static void go_scenes_cb(lv_event_t *e)
{
    (void)e;
    hub_ui_go(HUB_ROUTE_SCENES);
}

static void subpage_cb(lv_event_t *e)
{
    /* Kept for potential UI chrome; primary paging is hub_nav swipe. */
    int dir = (int)(intptr_t)lv_event_get_user_data(e);
    hub_model_t *m = hub_model();
    int enabled = hub_model_enabled_widget_count(m->room_idx);
    int pages = enabled > 0 ? (enabled + PAGE_SIZE - 1) / PAGE_SIZE : 1;
    if (dir > 0) {
        if (m->room_subpage < pages - 1) {
            m->room_subpage++;
            hub_ui_refresh();
        } else {
            hub_ui_go(HUB_ROUTE_SCENES);
        }
    } else {
        if (m->room_subpage > 0) {
            m->room_subpage--;
            hub_ui_refresh();
        } else {
            hub_ui_go(HUB_ROUTE_HOME);
        }
    }
}

static void toggle_slot_cb(lv_event_t *e)
{
    w_ud_t *u = (w_ud_t *)lv_event_get_user_data(e);
    hub_model_toggle_widget(u->room, u->slot);
    hub_ui_refresh();
}

static void step_slot_cb(lv_event_t *e)
{
    w_ud_t *u = (w_ud_t *)lv_event_get_user_data(e);
    hub_model_step_widget(u->room, u->slot, u->delta);
    hub_ui_refresh();
}

static void set_level_cb(lv_event_t *e)
{
    w_ud_t *u = (w_ud_t *)lv_event_get_user_data(e);
    hub_model_set_widget_level(u->room, u->slot, u->delta);
    hub_ui_refresh();
}

static void enable_slot_cb(lv_event_t *e)
{
    w_ud_t *u = (w_ud_t *)lv_event_get_user_data(e);
    hub_widget_t *w = hub_model_widget_by_slot(u->room, u->slot);
    if (w) {
        hub_model_set_widget_enabled(u->room, u->slot, !w->enabled);
        hub_ui_refresh();
    }
}

static void remove_slot_cb(lv_event_t *e)
{
    w_ud_t *u = (w_ud_t *)lv_event_get_user_data(e);
    hub_model_remove_widget(u->room, u->slot);
    hub_ui_refresh();
}

static void add_type_cb(lv_event_t *e)
{
    hub_wtype_t t = (hub_wtype_t)(uintptr_t)lv_event_get_user_data(e);
    hub_model_add_widget(hub_model()->room_idx, t);
    hub_ui_refresh();
}

static hub_ico_t ico_for(hub_wtype_t t)
{
    switch (t) {
    case HUB_W_ONOFF:
    case HUB_W_DIMMER:
        return HUB_ICO_BULB;
    case HUB_W_CURTAIN:
        return HUB_ICO_CURTAIN;
    case HUB_W_SHUTTER:
        return HUB_ICO_SHUTTER;
    case HUB_W_CLIM:
        return HUB_ICO_SNOW;
    case HUB_W_THERMO:
        return HUB_ICO_HEAT;
    case HUB_W_FAN:
        return HUB_ICO_FAN;
    case HUB_W_PLUG:
        return HUB_ICO_PLUG;
    default:
        return HUB_ICO_HOME;
    }
}

static bool widget_active(const hub_widget_t *w)
{
    if (!w) {
        return false;
    }
    if (w->type == HUB_W_CURTAIN || w->type == HUB_W_SHUTTER) {
        return w->level > 50;
    }
    if (w->type == HUB_W_DIMMER) {
        return w->on && w->level > 0;
    }
    return w->on;
}

static lv_obj_t *mk_lbl(lv_obj_t *par, const char *txt, lv_color_t c)
{
    lv_obj_t *l = lv_label_create(par);
    lv_label_set_text(l, txt ? txt : "");
    hub_style_label(l, c, hub_font());
    return l;
}

static int slot_of(int room, const hub_widget_t *w)
{
    hub_model_t *m = hub_model();
    for (int i = 0; i < m->widget_count[room]; i++) {
        if (&m->widgets[room][i] == w) {
            return i;
        }
    }
    return -1;
}

typedef struct {
    lv_coord_t card_r;
    lv_coord_t badge_r;
    lv_coord_t pwr_r;
    lv_coord_t ctrl_r;
    bool accent_bar;
    bool zen_hairline;
} tw_chrome_t;

static void tw_chrome(tw_chrome_t *c)
{
    const hub_palette_t *p = hub_palette();
    const char *n = p->name ? p->name : "";
    memset(c, 0, sizeof(*c));
    c->card_r = p->radius;
    c->badge_r = 12;
    c->pwr_r = LV_RADIUS_CIRCLE;
    c->ctrl_r = 12;
    c->accent_bar = true;

    if (strcmp(n, "zen") == 0) {
        c->card_r = 0;
        c->badge_r = 0;
        c->pwr_r = 0;
        c->ctrl_r = 0;
        c->accent_bar = false;
        c->zen_hairline = true;
    } else if (strcmp(n, "bloom") == 0) {
        /* FE bloom: pill everything, no left accent bar */
        c->card_r = 28;
        c->badge_r = LV_RADIUS_CIRCLE;
        c->pwr_r = LV_RADIUS_CIRCLE;
        c->ctrl_r = LV_RADIUS_CIRCLE;
        c->accent_bar = false;
    } else if (strcmp(n, "metro") == 0) {
        c->card_r = 0;
        c->badge_r = 0;
        c->pwr_r = 0;
        c->ctrl_r = 0;
        c->accent_bar = false;
    } else if (strcmp(n, "ink") == 0) {
        c->card_r = 4;
        c->badge_r = 0;
        c->pwr_r = 0;
        c->ctrl_r = 0;
        c->accent_bar = true;
    } else if (strcmp(n, "pulse") == 0) {
        c->card_r = 0;
        c->badge_r = 0;
        c->pwr_r = 0;
        c->ctrl_r = 0;
        c->accent_bar = true;
    } else if (strcmp(n, "dusk") == 0) {
        c->badge_r = 14;
        c->pwr_r = 14;
        c->ctrl_r = 14;
    } else if (strcmp(n, "slate") == 0) {
        /* Left accent via theme_card_chrome border — skip bar */
        c->accent_bar = false;
        c->badge_r = 10;
        c->ctrl_r = 10;
    } else if (strcmp(n, "sand") == 0 || strcmp(n, "forest") == 0 || strcmp(n, "ocean") == 0) {
        c->badge_r = 12;
        c->ctrl_r = 12;
    }
}

static void style_card(lv_obj_t *obj, bool active)
{
    const hub_palette_t *p = hub_palette();
    const char *n = p->name ? p->name : "";
    tw_chrome_t ch;
    tw_chrome(&ch);

    hub_apply_card(obj, false);
    theme_card_chrome(obj, active);
    lv_obj_set_style_radius(obj, ch.card_r, 0);

    if (ch.zen_hairline) {
        lv_obj_set_style_radius(obj, 0, 0);
        lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(obj, 0, 0);
        lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_BOTTOM, 0);
        lv_obj_set_style_border_width(obj, 1, 0);
        lv_obj_set_style_border_color(obj, active ? p->t1 : p->line, 0);
        lv_obj_set_style_pad_ver(obj, 14, 0);
        lv_obj_set_style_pad_hor(obj, 4, 0);
        lv_obj_set_style_shadow_width(obj, 0, 0);
        return;
    }

    lv_obj_set_style_pad_all(obj, 10, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);

    if (strcmp(n, "bloom") == 0) {
        /* Soft frosted card — FE .w bloom */
        lv_obj_set_style_bg_color(obj, p->bg_card, 0);
        lv_obj_set_style_border_width(obj, 1, 0);
        lv_obj_set_style_border_color(obj, lv_color_make(255, 255, 255), 0);
        lv_obj_set_style_border_opa(obj, LV_OPA_60, 0);
        lv_obj_set_style_shadow_width(obj, 18, 0);
        lv_obj_set_style_shadow_opa(obj, LV_OPA_20, 0);
        lv_obj_set_style_shadow_ofs_y(obj, 6, 0);
        lv_obj_set_style_shadow_color(obj, lv_color_make(90, 60, 80), 0);
        if (active) {
            lv_obj_set_style_bg_color(obj, lv_color_mix(p->accent, p->bg_card, LV_OPA_30), 0);
            lv_obj_set_style_border_color(obj, lv_color_mix(p->accent, p->line, LV_OPA_50), 0);
            lv_obj_set_style_border_opa(obj, LV_OPA_COVER, 0);
        }
        return;
    }
    if (strcmp(n, "metro") == 0) {
        lv_obj_set_style_border_width(obj, 0, 0);
        lv_obj_set_style_shadow_width(obj, 0, 0);
        lv_obj_set_style_bg_color(obj, active ? lv_color_make(58, 58, 58)
                                              : lv_color_make(45, 45, 45), 0);
        return;
    }
    if (strcmp(n, "ink") == 0) {
        lv_obj_set_style_bg_color(obj, active ? lv_color_make(17, 17, 17) : p->bg_card, 0);
        lv_obj_set_style_border_width(obj, active ? 1 : 1, 0);
        lv_obj_set_style_border_color(obj, active ? p->accent : p->line, 0);
        lv_obj_set_style_shadow_width(obj, 0, 0);
        return;
    }
    if (strcmp(n, "pulse") == 0) {
        lv_obj_set_style_bg_color(obj, p->bg_card, 0);
        lv_obj_set_style_border_width(obj, 1, 0);
        lv_obj_set_style_border_color(obj, active ? p->accent : p->line, 0);
        lv_obj_set_style_shadow_width(obj, 0, 0);
        return;
    }
    if (strcmp(n, "dusk") == 0) {
        lv_obj_set_style_bg_color(obj, p->bg_card, 0);
        lv_obj_set_style_border_width(obj, 1, 0);
        lv_obj_set_style_border_color(obj, lv_color_make(220, 200, 255), 0);
        lv_obj_set_style_border_opa(obj, LV_OPA_30, 0);
        lv_obj_set_style_shadow_width(obj, 20, 0);
        lv_obj_set_style_shadow_opa(obj, LV_OPA_30, 0);
        lv_obj_set_style_shadow_ofs_y(obj, 8, 0);
        if (active) {
            lv_obj_set_style_border_color(obj, p->accent, 0);
            lv_obj_set_style_border_opa(obj, LV_OPA_COVER, 0);
        }
        return;
    }
    if (strcmp(n, "sand") == 0) {
        lv_obj_set_style_bg_color(obj, p->bg_card, 0);
        lv_obj_set_style_border_width(obj, 1, 0);
        lv_obj_set_style_border_color(obj, active ? p->accent : p->line, 0);
        lv_obj_set_style_shadow_width(obj, 14, 0);
        lv_obj_set_style_shadow_opa(obj, LV_OPA_20, 0);
        lv_obj_set_style_shadow_ofs_y(obj, 4, 0);
        lv_obj_set_style_shadow_color(obj, lv_color_make(60, 45, 30), 0);
        return;
    }
    if (strcmp(n, "forest") == 0) {
        lv_obj_set_style_bg_color(obj, active ? lv_color_mix(p->accent, p->bg_card, LV_OPA_20)
                                              : p->bg_card, 0);
        lv_obj_set_style_border_width(obj, active ? 1 : 1, 0);
        lv_obj_set_style_border_color(obj, active ? p->accent : p->line, 0);
        lv_obj_set_style_shadow_width(obj, 16, 0);
        lv_obj_set_style_shadow_opa(obj, LV_OPA_30, 0);
        return;
    }

    /* slate / ocean / default */
    lv_obj_set_style_bg_color(obj, active ? p->bg_card : p->bg_card, 0);
    if (active) {
        lv_obj_set_style_border_color(obj, p->accent, 0);
        lv_obj_set_style_border_width(obj, 1, 0);
        lv_obj_set_style_shadow_width(obj, 14, 0);
        lv_obj_set_style_shadow_opa(obj, LV_OPA_20, 0);
        lv_obj_set_style_shadow_ofs_y(obj, 4, 0);
        lv_obj_set_style_shadow_color(obj, p->t1, 0);
    }
}

/** FE .w::before — only themes that use a left accent rail */
static void add_accent_bar(lv_obj_t *cell, bool on)
{
    if (!on) {
        return;
    }
    tw_chrome_t ch;
    tw_chrome(&ch);
    if (!ch.accent_bar) {
        return;
    }
    const hub_palette_t *p = hub_palette();
    lv_obj_t *bar = lv_obj_create(cell);
    lv_obj_remove_style_all(bar);
    lv_coord_t w = (p->name && strcmp(p->name, "ink") == 0) ? 2 : 3;
    lv_obj_set_size(bar, w, LV_PCT(72));
    lv_obj_set_style_radius(bar, 2, 0);
    lv_obj_set_style_bg_color(bar, p->accent, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(bar, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_FLOATING);
    lv_obj_align(bar, LV_ALIGN_LEFT_MID, 0, 0);
}

static lv_obj_t *add_badge(lv_obj_t *cell, hub_ico_t ico, bool on, bool compact)
{
    const hub_palette_t *p = hub_palette();
    tw_chrome_t ch;
    tw_chrome(&ch);
    lv_coord_t sz = compact ? 32 : 40;
    lv_obj_t *badge = lv_obj_create(cell);
    lv_obj_remove_style_all(badge);
    lv_obj_set_size(badge, sz, sz);
    lv_obj_set_style_radius(badge, ch.badge_r, 0);
    lv_obj_set_style_bg_opa(badge, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(badge, on ? lv_color_mix(p->accent, p->bg_card, LV_OPA_20) : p->bg_card2, 0);
    lv_obj_clear_flag(badge, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    hub_ico_add(badge, ico, on ? p->accent : p->t3, compact ? 16 : 20);
    lv_obj_center(lv_obj_get_child(badge, 0));
    return badge;
}

static lv_obj_t *add_pwr_btn(lv_obj_t *cell, bool on, int room, int slot, bool compact)
{
    const hub_palette_t *p = hub_palette();
    tw_chrome_t ch;
    tw_chrome(&ch);
    lv_coord_t sz = compact ? 30 : 36;
    lv_obj_t *btn = lv_btn_create(cell);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, sz, sz);
    lv_obj_set_style_radius(btn, ch.pwr_r, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    if (on) {
        lv_obj_set_style_bg_color(btn, p->accent, 0);
    } else {
        lv_obj_set_style_bg_color(btn, p->bg_card2, 0);
        lv_obj_set_style_border_width(btn, 1, 0);
        lv_obj_set_style_border_color(btn, p->line, 0);
    }
    lv_obj_add_event_cb(btn, toggle_slot_cb, LV_EVENT_CLICKED, ud_alloc(room, slot, 0));
    hub_ico_add(btn, HUB_ICO_POWER, on ? p->ink_on : p->t2, compact ? 12 : 14);
    lv_obj_center(lv_obj_get_child(btn, 0));
    return btn;
}

static lv_obj_t *mk_flex_row(lv_obj_t *par)
{
    lv_obj_t *row = lv_obj_create(par);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return row;
}

static lv_obj_t *mk_spacer(lv_obj_t *par)
{
    lv_obj_t *sp = lv_obj_create(par);
    lv_obj_remove_style_all(sp);
    lv_obj_set_width(sp, LV_PCT(100));
    lv_obj_set_height(sp, 0);
    lv_obj_set_flex_grow(sp, 1);
    lv_obj_clear_flag(sp, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    return sp;
}

static lv_obj_t *mk_title_lbl(lv_obj_t *par, const char *txt, lv_color_t c, bool compact)
{
    lv_obj_t *l = mk_lbl(par, txt, c);
    lv_obj_set_style_text_font(l, hub_font(), 0);
    lv_label_set_long_mode(l, LV_LABEL_LONG_DOT);
    lv_obj_set_width(l, compact ? 90 : 140);
    return l;
}

static void style_ctrl_btn(lv_obj_t *b, bool active, bool zen)
{
    const hub_palette_t *p = hub_palette();
    tw_chrome_t ch;
    tw_chrome(&ch);
    style_card(b, active);
    if (zen) {
        lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
        if (active) {
            lv_obj_set_style_bg_color(b, p->t1, 0);
            lv_obj_set_style_border_color(b, p->t1, 0);
        } else {
            lv_obj_set_style_bg_opa(b, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_width(b, 1, 0);
            lv_obj_set_style_border_side(b, LV_BORDER_SIDE_FULL, 0);
            lv_obj_set_style_border_color(b, p->line, 0);
        }
    } else if (p->name && strcmp(p->name, "metro") == 0) {
        lv_obj_set_style_border_width(b, 0, 0);
        lv_obj_set_style_bg_color(b, active ? lv_color_make(0, 188, 242)
                                            : lv_color_make(58, 58, 58), 0);
    }
    lv_obj_set_style_radius(b, ch.ctrl_r, 0);
}

static void slider_cb(lv_event_t *e)
{
    w_ud_t *u = (w_ud_t *)lv_event_get_user_data(e);
    lv_obj_t *sl = lv_event_get_target(e);
    int val = (int)lv_slider_get_value(sl);
    hub_model_set_widget_level(u->room, u->slot, val);
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        hub_ui_refresh();
    }
}

static void paint_widget(lv_obj_t *cell, hub_widget_t *w, int room, bool compact)
{
    const hub_palette_t *p = hub_palette();
    int slot = slot_of(room, w);
    if (slot < 0) {
        return;
    }
    bool on = widget_active(w);
    bool zen = p->name && strcmp(p->name, "zen") == 0;
    style_card(cell, on);
    lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(cell, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cell, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(cell, compact ? 4 : 6, 0);
    lv_obj_set_style_pad_left(cell, 10, 0);
    lv_obj_set_style_pad_right(cell, 8, 0);
    lv_obj_set_style_pad_top(cell, compact ? 6 : 8, 0);
    lv_obj_set_style_pad_bottom(cell, compact ? 6 : 8, 0);
    add_accent_bar(cell, on);

    const char *wname = hub_model_widget_label(w);
    char val[24];

    if (w->type == HUB_W_ONOFF || w->type == HUB_W_FAN || w->type == HUB_W_PLUG) {
        lv_obj_t *hdr = mk_flex_row(cell);
        add_badge(hdr, ico_for(w->type), on, compact);
        add_pwr_btn(hdr, w->on, room, slot, compact);
        mk_spacer(cell);
        mk_title_lbl(cell, wname, p->t1, compact);
        snprintf(val, sizeof(val), "%s", w->on ? "ON" : "OFF");
        mk_lbl(cell, val, w->on ? p->accent : p->t3);
        lv_obj_add_flag(cell, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(cell, toggle_slot_cb, LV_EVENT_CLICKED, ud_alloc(room, slot, 0));
    } else if (w->type == HUB_W_DIMMER) {
        lv_obj_t *hdr = mk_flex_row(cell);
        add_badge(hdr, HUB_ICO_BULB, on, compact);
        add_pwr_btn(hdr, w->on, room, slot, compact);
        mk_spacer(cell);

        lv_obj_t *meta = mk_flex_row(cell);
        mk_title_lbl(meta, wname, p->t1, compact);
        snprintf(val, sizeof(val), "%d%%", w->on ? w->level : 0);
        mk_lbl(meta, val, on ? p->accent : p->t4);

        lv_obj_t *sl = lv_slider_create(cell);
        lv_obj_set_width(sl, LV_PCT(100));
        lv_obj_set_height(sl, compact ? 8 : 10);
        lv_slider_set_range(sl, 0, 100);
        lv_slider_set_value(sl, w->on ? w->level : 0, LV_ANIM_OFF);
        if (zen) {
            lv_obj_set_style_radius(sl, 0, LV_PART_MAIN);
            lv_obj_set_style_radius(sl, 0, LV_PART_INDICATOR);
            lv_obj_set_style_radius(sl, 0, LV_PART_KNOB);
            lv_obj_set_style_bg_color(sl, lv_color_mix(p->t1, p->bg_deep, LV_OPA_10), LV_PART_MAIN);
            lv_obj_set_style_bg_color(sl, p->t1, LV_PART_INDICATOR);
            lv_obj_set_style_bg_color(sl, p->t1, LV_PART_KNOB);
            lv_obj_set_style_pad_ver(sl, 4, LV_PART_KNOB);
            lv_obj_set_style_pad_hor(sl, 0, LV_PART_KNOB);
            lv_obj_set_style_width(sl, 3, LV_PART_KNOB);
            lv_obj_set_style_shadow_width(sl, 0, LV_PART_KNOB);
        } else if (p->name && (strcmp(p->name, "ink") == 0 || strcmp(p->name, "pulse") == 0 ||
                               strcmp(p->name, "metro") == 0)) {
            lv_obj_set_style_radius(sl, 0, LV_PART_MAIN);
            lv_obj_set_style_radius(sl, 0, LV_PART_INDICATOR);
            lv_obj_set_style_radius(sl, 0, LV_PART_KNOB);
            lv_obj_set_style_bg_color(sl, p->bg_elev, LV_PART_MAIN);
            lv_obj_set_style_bg_color(sl, p->accent, LV_PART_INDICATOR);
            lv_obj_set_style_bg_color(sl, p->name && strcmp(p->name, "ink") == 0 ? p->accent : p->bg_card,
                                     LV_PART_KNOB);
            lv_obj_set_style_pad_all(sl, 3, LV_PART_KNOB);
        } else if (p->name && strcmp(p->name, "bloom") == 0) {
            lv_obj_set_style_radius(sl, LV_RADIUS_CIRCLE, LV_PART_MAIN);
            lv_obj_set_style_radius(sl, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
            lv_obj_set_style_radius(sl, LV_RADIUS_CIRCLE, LV_PART_KNOB);
            lv_obj_set_style_bg_color(sl, p->bg_elev, LV_PART_MAIN);
            lv_obj_set_style_bg_color(sl, p->accent, LV_PART_INDICATOR);
            lv_obj_set_style_bg_color(sl, lv_color_make(255, 255, 255), LV_PART_KNOB);
            lv_obj_set_style_pad_all(sl, 5, LV_PART_KNOB);
            lv_obj_set_style_shadow_width(sl, 8, LV_PART_KNOB);
            lv_obj_set_style_shadow_opa(sl, LV_OPA_30, LV_PART_KNOB);
        } else {
            lv_obj_set_style_bg_color(sl, p->bg_elev, LV_PART_MAIN);
            lv_obj_set_style_bg_color(sl, p->accent, LV_PART_INDICATOR);
            lv_obj_set_style_bg_color(sl, p->bg_card, LV_PART_KNOB);
            lv_obj_set_style_pad_all(sl, 4, LV_PART_KNOB);
            if (p->name && strcmp(p->name, "dusk") == 0) {
                lv_obj_set_style_radius(sl, LV_RADIUS_CIRCLE, LV_PART_MAIN);
                lv_obj_set_style_radius(sl, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
                lv_obj_set_style_radius(sl, LV_RADIUS_CIRCLE, LV_PART_KNOB);
            }
        }
        lv_obj_add_event_cb(sl, slider_cb, LV_EVENT_VALUE_CHANGED, ud_alloc(room, slot, 0));
        lv_obj_add_event_cb(sl, slider_cb, LV_EVENT_RELEASED, ud_alloc(room, slot, 0));
        if (!w->on) {
            lv_obj_set_style_opa(sl, LV_OPA_40, 0);
        }
    } else if (w->type == HUB_W_CURTAIN || w->type == HUB_W_SHUTTER) {
        bool is_curtain = (w->type == HUB_W_CURTAIN);
        lv_obj_t *hdr = lv_obj_create(cell);
        lv_obj_remove_style_all(hdr);
        lv_obj_set_width(hdr, LV_PCT(100));
        lv_obj_set_height(hdr, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(hdr, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(hdr, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(hdr, 8, 0);
        lv_obj_clear_flag(hdr, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

        add_badge(hdr, ico_for(w->type), on, compact);
        lv_obj_t *txt = lv_obj_create(hdr);
        lv_obj_remove_style_all(txt);
        lv_obj_set_flex_grow(txt, 1);
        lv_obj_set_height(txt, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(txt, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_row(txt, 2, 0);
        lv_obj_clear_flag(txt, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
        mk_title_lbl(txt, wname, p->t1, compact);
        snprintf(val, sizeof(val), "%d%%", w->level);
        mk_lbl(txt, val, p->t3);

        mk_spacer(cell);

        lv_obj_t *btns = lv_obj_create(cell);
        lv_obj_remove_style_all(btns);
        lv_obj_set_width(btns, LV_PCT(100));
        lv_obj_set_height(btns, compact ? 32 : 36);
        lv_obj_set_flex_flow(btns, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(btns, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(btns, compact ? 6 : 8, 0);
        lv_obj_clear_flag(btns, LV_OBJ_FLAG_SCROLLABLE);

        const int levels[] = { 100, 50, 0 };
        hub_ico_t icos[3];
        if (is_curtain) {
            icos[0] = HUB_ICO_LEFT;
            icos[1] = HUB_ICO_STOP;
            icos[2] = HUB_ICO_RIGHT;
        } else {
            icos[0] = HUB_ICO_UP;
            icos[1] = HUB_ICO_STOP;
            icos[2] = HUB_ICO_DOWN;
        }
        for (int i = 0; i < 3; i++) {
            lv_obj_t *b = lv_btn_create(btns);
            lv_obj_remove_style_all(b);
            style_ctrl_btn(b, w->level == levels[i], zen);
            lv_obj_set_flex_grow(b, 1);
            lv_obj_set_height(b, LV_PCT(100));
            lv_obj_add_event_cb(b, set_level_cb, LV_EVENT_CLICKED, ud_alloc(room, slot, levels[i]));
            hub_ico_add(b, icos[i],
                        (zen && w->level == levels[i]) ? p->ink_on
                        : (w->level == levels[i] ? p->accent : p->t2),
                        14);
            lv_obj_center(lv_obj_get_child(b, 0));
        }
    } else if (w->type == HUB_W_CLIM || w->type == HUB_W_THERMO) {
        lv_color_t hue = w->type == HUB_W_CLIM ? p->cool : p->heat;
        lv_obj_t *hdr = mk_flex_row(cell);
        add_badge(hdr, ico_for(w->type), on, compact);
        add_pwr_btn(hdr, w->on, room, slot, compact);

        mk_spacer(cell);

        snprintf(val, sizeof(val), w->on ? "%d°" : "--", w->level);
        lv_obj_t *deg = lv_label_create(cell);
        lv_label_set_text(deg, val);
        hub_style_label(deg, on ? hue : p->t4, compact ? hub_font() : hub_font_clock());
        lv_obj_set_width(deg, LV_PCT(100));
        lv_obj_set_style_text_align(deg, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t *foot = mk_flex_row(cell);
        mk_title_lbl(foot, wname, p->t1, compact);

        lv_obj_t *steps = lv_obj_create(foot);
        lv_obj_remove_style_all(steps);
        lv_obj_set_size(steps, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(steps, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_column(steps, 6, 0);
        lv_obj_clear_flag(steps, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *am = lv_btn_create(steps);
        lv_obj_remove_style_all(am);
        style_ctrl_btn(am, false, zen);
        lv_obj_set_size(am, 36, 32);
        if (zen) {
            lv_obj_set_style_bg_opa(am, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_side(am, LV_BORDER_SIDE_FULL, 0);
            lv_obj_set_style_border_width(am, 1, 0);
            lv_obj_set_style_border_color(am, p->line, 0);
        }
        lv_obj_add_event_cb(am, step_slot_cb, LV_EVENT_CLICKED, ud_alloc(room, slot, -1));
        hub_ico_add(am, HUB_ICO_MINUS, p->t1, 14);
        lv_obj_center(lv_obj_get_child(am, 0));

        lv_obj_t *ap = lv_btn_create(steps);
        lv_obj_remove_style_all(ap);
        style_ctrl_btn(ap, false, zen);
        lv_obj_set_size(ap, 36, 32);
        if (zen) {
            lv_obj_set_style_bg_opa(ap, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_side(ap, LV_BORDER_SIDE_FULL, 0);
            lv_obj_set_style_border_width(ap, 1, 0);
            lv_obj_set_style_border_color(ap, p->line, 0);
        }
        lv_obj_add_event_cb(ap, step_slot_cb, LV_EVENT_CLICKED, ud_alloc(room, slot, 1));
        hub_ico_add(ap, HUB_ICO_PLUS, p->t1, 14);
        lv_obj_center(lv_obj_get_child(ap, 0));
    }
}

static void layout_grid(lv_obj_t *body, hub_widget_t **ws, int n, int room)
{
    if (n <= 0) {
        return;
    }
    lv_obj_t *grid = lv_obj_create(body);
    lv_obj_remove_style_all(grid);
    lv_obj_set_width(grid, LV_PCT(100));
    lv_obj_set_flex_grow(grid, 1);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_row(grid, 6, 0);
    lv_obj_set_style_pad_column(grid, 6, 0);
    lv_obj_clear_flag(grid, LV_OBJ_FLAG_SCROLLABLE);

    static lv_coord_t c1[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static lv_coord_t c2[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static lv_coord_t r1[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static lv_coord_t r2[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static lv_coord_t r3[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };

    bool compact = (n >= 4);

    if (n == 1) {
        lv_obj_set_grid_dsc_array(grid, c1, r1);
        lv_obj_t *cell = lv_obj_create(grid);
        lv_obj_remove_style_all(cell);
        lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
        paint_widget(cell, ws[0], room, false);
    } else if (n == 2) {
        lv_obj_set_grid_dsc_array(grid, c1, r2);
        for (int i = 0; i < 2; i++) {
            lv_obj_t *cell = lv_obj_create(grid);
            lv_obj_remove_style_all(cell);
            lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, i, 1);
            paint_widget(cell, ws[i], room, false);
        }
    } else if (n == 3) {
        lv_obj_set_grid_dsc_array(grid, c1, r3);
        for (int i = 0; i < 3; i++) {
            lv_obj_t *cell = lv_obj_create(grid);
            lv_obj_remove_style_all(cell);
            lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, i, 1);
            paint_widget(cell, ws[i], room, true);
        }
    } else if (n == 4) {
        lv_obj_set_grid_dsc_array(grid, c2, r2);
        for (int i = 0; i < 4; i++) {
            lv_obj_t *cell = lv_obj_create(grid);
            lv_obj_remove_style_all(cell);
            lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, i % 2, 1, LV_GRID_ALIGN_STRETCH, i / 2, 1);
            paint_widget(cell, ws[i], room, compact);
        }
    } else if (n == 5) {
        /* wide top + 2x2 bottom */
        static lv_coord_t r5[] = { LV_GRID_FR(12), LV_GRID_FR(10), LV_GRID_FR(10), LV_GRID_TEMPLATE_LAST };
        lv_obj_set_grid_dsc_array(grid, c2, r5);
        lv_obj_t *top = lv_obj_create(grid);
        lv_obj_remove_style_all(top);
        lv_obj_set_grid_cell(top, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
        paint_widget(top, ws[0], room, true);
        for (int i = 1; i < 5; i++) {
            lv_obj_t *cell = lv_obj_create(grid);
            lv_obj_remove_style_all(cell);
            int col = (i - 1) % 2;
            int row = 1 + (i - 1) / 2;
            lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
            paint_widget(cell, ws[i], room, true);
        }
    } else {
        static lv_coord_t r6[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
        lv_obj_set_grid_dsc_array(grid, c2, r6);
        for (int i = 0; i < 6; i++) {
            lv_obj_t *cell = lv_obj_create(grid);
            lv_obj_remove_style_all(cell);
            lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, i % 2, 1, LV_GRID_ALIGN_STRETCH, i / 2, 1);
            paint_widget(cell, ws[i], room, true);
        }
    }
}

void hub_toast_present(lv_obj_t *parent)
{
    char msg[HUB_TOAST_MAX];
    if (!hub_model_take_toast(msg, sizeof(msg))) {
        return;
    }
    const hub_palette_t *p = hub_palette();
    lv_obj_t *toast = lv_obj_create(parent);
    lv_obj_remove_style_all(toast);
    lv_obj_set_size(toast, 280, 40);
    lv_obj_set_style_radius(toast, (p->name && strcmp(p->name, "zen") == 0) ? 0 : 10, 0);
    lv_obj_set_style_bg_color(toast, p->bg_elev, 0);
    lv_obj_set_style_bg_opa(toast, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(toast, 1, 0);
    lv_obj_set_style_border_color(toast, p->accent, 0);
    lv_obj_add_flag(toast, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(toast, LV_ALIGN_BOTTOM_MID, 0, -28);
    lv_obj_t *l = lv_label_create(toast);
    lv_label_set_text(l, msg);
    hub_style_label(l, p->t1, hub_font());
    lv_obj_center(l);
    lv_obj_fade_out(toast, 400, 1600);
    lv_obj_del_delayed(toast, 2200);
}

void hub_build_room_page(lv_obj_t *parent)
{
    s_ud_n = 0;
    const hub_palette_t *p = hub_palette();
    hub_model_t *m = hub_model();
    int room = m->room_idx;
    int enabled = hub_model_enabled_widget_count(room);
    int pages = enabled > 0 ? (enabled + PAGE_SIZE - 1) / PAGE_SIZE : 1;
    if (m->room_subpage >= pages) {
        m->room_subpage = pages - 1;
    }
    if (m->room_subpage < 0) {
        m->room_subpage = 0;
    }
    bool multi = pages > 1;

    char title[40];
    if (multi) {
        snprintf(title, sizeof(title), "%s  %d/%d", hub_model_room_name(room),
                 m->room_subpage + 1, pages);
    } else {
        snprintf(title, sizeof(title), "%s", hub_model_room_name(room));
    }
    lv_obj_t *shell = hub_shell_create(parent, title, true);
    lv_obj_t *body = hub_shell_body(shell);
    hub_shell_set_dots(shell, multi ? pages : 4, multi ? m->room_subpage : 1);

    /* room tabs + edit */
    bool zen = p->name && strcmp(p->name, "zen") == 0;
    lv_obj_t *tabs = lv_obj_create(body);
    lv_obj_remove_style_all(tabs);
    lv_obj_set_width(tabs, LV_PCT(100));
    lv_obj_set_height(tabs, 32);
    lv_obj_set_flex_flow(tabs, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(tabs, 4, 0);
    for (int i = 0; i < HUB_ROOM_COUNT; i++) {
        lv_obj_t *t = lv_btn_create(tabs);
        lv_obj_remove_style_all(t);
        if (zen) {
            lv_obj_set_style_radius(t, 0, 0);
            lv_obj_set_style_border_width(t, 1, 0);
            lv_obj_set_style_border_color(t, p->line, 0);
            lv_obj_set_style_bg_opa(t, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(t, i == room ? p->accent : p->bg_card, 0);
        } else if (p->name && strcmp(p->name, "bloom") == 0) {
            lv_obj_set_style_radius(t, LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_bg_opa(t, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(t, i == room ? p->accent : p->bg_card, 0);
            lv_obj_set_style_border_width(t, i == room ? 0 : 1, 0);
            lv_obj_set_style_border_color(t, p->line, 0);
        } else {
            style_card(t, i == room);
            if (i == room) {
                lv_obj_set_style_bg_color(t, p->accent, 0);
            }
        }
        lv_obj_set_flex_grow(t, 1);
        lv_obj_set_height(t, LV_PCT(100));
        lv_obj_add_event_cb(t, go_room_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
        lv_obj_t *tl = lv_label_create(t);
        lv_label_set_text(tl, hub_model_room_name(i));
        hub_style_label(tl, i == room ? p->ink_on : p->t2, hub_font());
        lv_obj_center(tl);
    }
    lv_obj_t *ed = lv_btn_create(tabs);
    lv_obj_remove_style_all(ed);
    if (zen) {
        lv_obj_set_style_radius(ed, 0, 0);
        lv_obj_set_style_border_width(ed, 1, 0);
        lv_obj_set_style_border_color(ed, p->line, 0);
        lv_obj_set_style_bg_opa(ed, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(ed, p->bg_card, 0);
    } else {
        style_card(ed, false);
    }
    lv_obj_set_width(ed, 56);
    lv_obj_set_height(ed, LV_PCT(100));
    lv_obj_add_event_cb(ed, go_edit_cb, LV_EVENT_CLICKED, NULL);
    mk_lbl(ed, hub_tr("编辑", "Edit"), p->accent);
    lv_obj_center(lv_obj_get_child(ed, 0));

    if (enabled == 0) {
        mk_lbl(body, hub_tr("暂无控件", "No widgets"), p->t3);
        lv_obj_t *add = lv_btn_create(body);
        lv_obj_remove_style_all(add);
        style_card(add, true);
        lv_obj_set_style_bg_color(add, p->accent, 0);
        lv_obj_set_size(add, 180, 44);
        lv_obj_add_event_cb(add, go_edit_cb, LV_EVENT_CLICKED, NULL);
        mk_lbl(add, hub_tr("添加家居控件", "Add widgets"), p->ink_on);
        lv_obj_center(lv_obj_get_child(add, 0));
        return;
    }

    /* collect page widgets, clim/thermo first when n==5 */
    hub_widget_t *page_ws[PAGE_SIZE];
    int page_n = 0;
    int start = m->room_subpage * PAGE_SIZE;
    int seen = 0;
    for (int i = 0; i < m->widget_count[room] && page_n < PAGE_SIZE; i++) {
        if (!m->widgets[room][i].enabled) {
            continue;
        }
        if (seen++ < start) {
            continue;
        }
        page_ws[page_n++] = &m->widgets[room][i];
    }
    if (page_n == 5) {
        hub_widget_t *ordered[5];
        int o = 0;
        for (int i = 0; i < 5; i++) {
            if (page_ws[i]->type == HUB_W_CLIM || page_ws[i]->type == HUB_W_THERMO) {
                ordered[o++] = page_ws[i];
            }
        }
        for (int i = 0; i < 5; i++) {
            if (page_ws[i]->type != HUB_W_CLIM && page_ws[i]->type != HUB_W_THERMO) {
                ordered[o++] = page_ws[i];
            }
        }
        memcpy(page_ws, ordered, sizeof(ordered));
    }

    layout_grid(body, page_ws, page_n, room);
    /* FE: room sub-pages via swipe only — no prev/next chrome */
    (void)subpage_cb;
}

void hub_build_room_edit(lv_obj_t *parent)
{
    s_ud_n = 0;
    const hub_palette_t *p = hub_palette();
    hub_model_t *m = hub_model();
    int room = m->room_idx;

    char title[40];
    snprintf(title, sizeof(title), hub_tr("编辑 · %s", "Edit · %s"), hub_model_room_name(room));
    lv_obj_t *shell = hub_shell_create(parent, title, true);
    lv_obj_t *body = hub_shell_body(shell);

    int on_n = hub_model_enabled_widget_count(room);
    char head[48];
    snprintf(head, sizeof(head), hub_tr("%d / %d 显示", "%d / %d shown"), on_n, m->widget_count[room]);
    mk_lbl(body, head, p->t3);

    for (int i = 0; i < m->widget_count[room]; i++) {
        hub_widget_t *w = &m->widgets[room][i];
        lv_obj_t *row = lv_obj_create(body);
        lv_obj_remove_style_all(row);
        style_card(row, w->enabled);
        lv_obj_set_width(row, LV_PCT(100));
        lv_obj_set_height(row, 52);
        lv_obj_set_style_opa(row, w->enabled ? LV_OPA_COVER : LV_OPA_60, 0);

        hub_ico_add(row, ico_for(w->type), p->accent, 18);
        lv_obj_align(lv_obj_get_child(row, 0), LV_ALIGN_LEFT_MID, 8, 0);
        mk_lbl(row, hub_model_widget_label(w), p->t1);
        lv_obj_align(lv_obj_get_child(row, 1), LV_ALIGN_LEFT_MID, 36, -8);
        mk_lbl(row, hub_model_wtype_label(w->type), p->t3);
        lv_obj_align(lv_obj_get_child(row, 2), LV_ALIGN_LEFT_MID, 36, 10);

        lv_obj_t *vis = lv_btn_create(row);
        lv_obj_remove_style_all(vis);
        style_card(vis, w->enabled);
        lv_obj_set_size(vis, 52, 32);
        lv_obj_align(vis, LV_ALIGN_RIGHT_MID, -64, 0);
        lv_obj_add_event_cb(vis, enable_slot_cb, LV_EVENT_CLICKED, ud_alloc(room, i, 0));
        mk_lbl(vis, w->enabled ? hub_tr("显", "On") : hub_tr("隐", "Off"), p->t1);
        lv_obj_center(lv_obj_get_child(vis, 0));

        lv_obj_t *del = lv_btn_create(row);
        lv_obj_remove_style_all(del);
        style_card(del, false);
        lv_obj_set_size(del, 52, 32);
        lv_obj_align(del, LV_ALIGN_RIGHT_MID, -6, 0);
        lv_obj_add_event_cb(del, remove_slot_cb, LV_EVENT_CLICKED, ud_alloc(room, i, 0));
        mk_lbl(del, hub_tr("删", "Del"), p->alert);
        lv_obj_center(lv_obj_get_child(del, 0));
    }

    mk_lbl(body, hub_tr("添加控件", "Add widget"), p->t3);
    lv_obj_t *add_row = lv_obj_create(body);
    lv_obj_remove_style_all(add_row);
    lv_obj_set_width(add_row, LV_PCT(100));
    lv_obj_set_height(add_row, 80);
    lv_obj_set_flex_flow(add_row, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_row(add_row, 6, 0);
    lv_obj_set_style_pad_column(add_row, 6, 0);
    for (int t = 0; t < HUB_W_TYPE_COUNT; t++) {
        lv_obj_t *b = lv_btn_create(add_row);
        lv_obj_remove_style_all(b);
        style_card(b, false);
        lv_obj_set_size(b, 100, 34);
        lv_obj_add_event_cb(b, add_type_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)t);
        mk_lbl(b, hub_model_wtype_label((hub_wtype_t)t), p->t1);
        lv_obj_center(lv_obj_get_child(b, 0));
    }

    lv_obj_t *back = lv_btn_create(body);
    lv_obj_remove_style_all(back);
    style_card(back, true);
    lv_obj_set_style_bg_color(back, p->accent, 0);
    lv_obj_set_width(back, LV_PCT(100));
    lv_obj_set_height(back, 44);
    lv_obj_add_event_cb(back, go_room_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)room);
    mk_lbl(back, hub_tr("完成", "Done"), p->ink_on);
    lv_obj_center(lv_obj_get_child(back, 0));

}

static void toggle_living_type(hub_wtype_t type)
{
    hub_model_t *m = hub_model();
    for (int i = 0; i < m->widget_count[0]; i++) {
        if (m->widgets[0][i].type == type) {
            hub_model_toggle_widget(0, i);
            return;
        }
    }
}

static void app_toggle_cb(lv_event_t *e)
{
    int kind = (int)(uintptr_t)lv_event_get_user_data(e);
    hub_room_dev_t *d = &hub_model()->rooms[0];
    if (kind == 0) {
        d->pump_ok = !d->pump_ok;
        hub_model_toast(d->pump_ok ? hub_tr("水泵 On", "Pump on") : hub_tr("水泵 Off", "Pump off"));
    } else if (kind == 1) {
        d->valve_ok = !d->valve_ok;
        hub_model_toast(d->valve_ok ? hub_tr("水阀 On", "Valve on") : hub_tr("水阀 Off", "Valve off"));
    } else if (kind == 2) {
        toggle_living_type(HUB_W_PLUG);
        hub_model_toast(hub_tr("插座已切换", "Socket toggled"));
    } else {
        toggle_living_type(HUB_W_FAN);
        hub_model_toast(hub_tr("风扇已切换", "Fan toggled"));
    }
    hub_ui_refresh();
}

void hub_build_appliances(lv_obj_t *parent)
{
    const hub_palette_t *p = hub_palette();
    hub_room_dev_t *d = &hub_model()->rooms[0];
    lv_obj_t *shell = hub_shell_create(parent, hub_tr("设备", "Devices"), true);
    lv_obj_t *body = hub_shell_body(shell);

    struct {
        const char *zh;
        const char *en;
        bool on;
        int kind;
        hub_ico_t ico;
    } rows[] = {
        { "水泵 PUMP", "Pump", d->pump_ok, 0, HUB_ICO_DROP },
        { "水阀 VALVE", "Valve", d->valve_ok, 1, HUB_ICO_DROP },
        { "客厅插座", "Living socket", d->plug_on, 2, HUB_ICO_PLUG },
        { "客厅风扇", "Living fan", d->fan_on, 3, HUB_ICO_FAN },
    };
    for (int i = 0; i < 4; i++) {
        lv_obj_t *row = lv_btn_create(body);
        lv_obj_remove_style_all(row);
        style_card(row, rows[i].on);
        lv_obj_set_width(row, LV_PCT(100));
        lv_obj_set_height(row, 56);
        lv_obj_add_event_cb(row, app_toggle_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)rows[i].kind);
        hub_ico_add(row, rows[i].ico, p->accent, 20);
        lv_obj_align(lv_obj_get_child(row, 0), LV_ALIGN_LEFT_MID, 8, 0);
        mk_lbl(row, hub_tr(rows[i].zh, rows[i].en), p->t1);
        lv_obj_align(lv_obj_get_child(row, 1), LV_ALIGN_LEFT_MID, 40, 0);
        mk_lbl(row, rows[i].on ? "On" : "Off", rows[i].on ? p->on : p->t3);
        lv_obj_align(lv_obj_get_child(row, 2), LV_ALIGN_RIGHT_MID, -8, 0);
    }
}

/* ── Security (PIN-gated disarm) ─────────────────────────────────── */

static char s_pin[5];
static bool s_pin_mode; /* true = waiting for PIN to disarm */

static void arm_now_cb(lv_event_t *e)
{
    (void)e;
    hub_model_set_armed(true);
    hub_model_toast(hub_tr("已布防", "Armed"));
    s_pin[0] = '\0';
    s_pin_mode = false;
    hub_ui_refresh();
}

static void disarm_req_cb(lv_event_t *e)
{
    (void)e;
    s_pin_mode = true;
    s_pin[0] = '\0';
    hub_model_toast(hub_tr("输入 PIN 撤防", "Enter PIN to disarm"));
    hub_ui_refresh();
}

static void pin_key_cb(lv_event_t *e)
{
    const char *key = (const char *)lv_event_get_user_data(e);
    if (!key) {
        return;
    }
    if (key[0] == 'C') {
        s_pin[0] = '\0';
        hub_ui_refresh();
        return;
    }
    if (key[0] == 'O') {
        if (strcmp(s_pin, "1234") == 0) {
            hub_model_set_armed(false);
            hub_model_toast(hub_tr("已撤防", "Disarmed"));
            s_pin_mode = false;
        } else {
            hub_model_toast(hub_tr("PIN 错误", "Wrong PIN"));
        }
        s_pin[0] = '\0';
        hub_ui_refresh();
        return;
    }
    size_t n = strlen(s_pin);
    if (n < 4) {
        s_pin[n] = key[0];
        s_pin[n + 1] = '\0';
        hub_ui_refresh();
    }
}

void hub_build_security(lv_obj_t *parent)
{
    const hub_palette_t *p = hub_palette();
    hub_model_t *m = hub_model();
    bool zen = p->name && strcmp(p->name, "zen") == 0;
    lv_obj_t *shell = hub_shell_create(parent, hub_tr("安防", "Security"), true);
    lv_obj_t *body = hub_shell_body(shell);

    /* FE AlarmWidget page: centered badge + Security + Armed·Away */
    lv_obj_t *card = lv_btn_create(body);
    lv_obj_remove_style_all(card);
    if (zen) {
        lv_obj_set_style_radius(card, 0, 0);
        lv_obj_set_style_bg_opa(card, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_side(card, LV_BORDER_SIDE_BOTTOM, 0);
        lv_obj_set_style_border_width(card, 1, 0);
        lv_obj_set_style_border_color(card, m->armed ? p->t1 : p->line, 0);
    } else {
        style_card(card, m->armed);
    }
    lv_obj_set_width(card, LV_PCT(100));
    lv_obj_set_flex_grow(card, 1);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(card, 14, 0);
    lv_obj_add_event_cb(card, m->armed ? disarm_req_cb : arm_now_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *badge = lv_obj_create(card);
    lv_obj_remove_style_all(badge);
    lv_obj_set_size(badge, 96, 96);
    lv_obj_set_style_radius(badge, zen ? 0 : 20, 0);
    lv_obj_set_style_bg_opa(badge, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(badge,
                              lv_color_mix(m->armed ? p->on : p->t3, p->bg_card, LV_OPA_20), 0);
    lv_obj_clear_flag(badge, LV_OBJ_FLAG_CLICKABLE);
    hub_ico_add(badge, HUB_ICO_SHIELD, m->armed ? p->on : p->t3, 48);
    lv_obj_center(lv_obj_get_child(badge, 0));

    mk_lbl(card, hub_tr("安防", "Security"), p->t1);
    mk_lbl(card, m->armed ? hub_tr("已布防 · 离家", "Armed · Away") : hub_tr("已撤防", "Disarmed"),
           m->armed ? p->on : p->t3);

    lv_obj_t *row = lv_obj_create(body);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, 48);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(row, 8, 0);

    lv_obj_t *arm = lv_btn_create(row);
    lv_obj_remove_style_all(arm);
    if (zen) {
        lv_obj_set_style_radius(arm, 0, 0);
        lv_obj_set_style_bg_opa(arm, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(arm, p->accent, 0);
    } else {
        style_card(arm, true);
        lv_obj_set_style_bg_color(arm, p->accent, 0);
    }
    lv_obj_set_flex_grow(arm, 1);
    lv_obj_set_height(arm, LV_PCT(100));
    lv_obj_add_event_cb(arm, arm_now_cb, LV_EVENT_CLICKED, NULL);
    mk_lbl(arm, hub_tr("布防", "Arm"), p->ink_on);
    lv_obj_center(lv_obj_get_child(arm, 0));

    lv_obj_t *dis = lv_btn_create(row);
    lv_obj_remove_style_all(dis);
    if (zen) {
        lv_obj_set_style_radius(dis, 0, 0);
        lv_obj_set_style_border_width(dis, 1, 0);
        lv_obj_set_style_border_color(dis, p->line, 0);
        lv_obj_set_style_bg_opa(dis, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(dis, p->bg_card, 0);
    } else {
        style_card(dis, false);
    }
    lv_obj_set_flex_grow(dis, 1);
    lv_obj_set_height(dis, LV_PCT(100));
    lv_obj_add_event_cb(dis, disarm_req_cb, LV_EVENT_CLICKED, NULL);
    mk_lbl(dis, hub_tr("撤防 PIN", "Disarm PIN"), p->t1);
    lv_obj_center(lv_obj_get_child(dis, 0));

    if (s_pin_mode) {
        lv_obj_t *dots = lv_obj_create(body);
        lv_obj_remove_style_all(dots);
        lv_obj_set_width(dots, LV_PCT(100));
        lv_obj_set_height(dots, 24);
        lv_obj_set_flex_flow(dots, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(dots, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(dots, 12, 0);
        size_t pn = strlen(s_pin);
        for (int i = 0; i < 4; i++) {
            lv_obj_t *d = lv_obj_create(dots);
            lv_obj_remove_style_all(d);
            lv_obj_set_size(d, 12, 12);
            lv_obj_set_style_radius(d, zen ? 0 : 2, 0);
            if ((size_t)i < pn) {
                lv_obj_set_style_bg_color(d, p->accent, 0);
                lv_obj_set_style_bg_opa(d, LV_OPA_COVER, 0);
            } else {
                lv_obj_set_style_border_width(d, 2, 0);
                lv_obj_set_style_border_color(d, p->t4, 0);
            }
        }

        lv_obj_t *pad = lv_obj_create(body);
        lv_obj_remove_style_all(pad);
        lv_obj_set_width(pad, LV_PCT(100));
        lv_obj_set_height(pad, 200);
        lv_obj_set_layout(pad, LV_LAYOUT_GRID);
        lv_obj_set_style_pad_row(pad, 6, 0);
        lv_obj_set_style_pad_column(pad, 6, 0);
        static lv_coord_t cols[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
        static lv_coord_t rows[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
        lv_obj_set_grid_dsc_array(pad, cols, rows);
        const char *keys[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "C", "0", "O" };
        for (int i = 0; i < 12; i++) {
            lv_obj_t *k = lv_btn_create(pad);
            lv_obj_remove_style_all(k);
            if (zen) {
                lv_obj_set_style_radius(k, 0, 0);
                lv_obj_set_style_border_width(k, 1, 0);
                lv_obj_set_style_border_color(k, p->line, 0);
                lv_obj_set_style_bg_opa(k, LV_OPA_COVER, 0);
                lv_obj_set_style_bg_color(k, p->bg_card, 0);
            } else {
                style_card(k, false);
            }
            lv_obj_set_grid_cell(k, LV_GRID_ALIGN_STRETCH, i % 3, 1, LV_GRID_ALIGN_STRETCH, i / 3, 1);
            lv_obj_add_event_cb(k, pin_key_cb, LV_EVENT_CLICKED, (void *)keys[i]);
            const char *lab = keys[i];
            if (keys[i][0] == 'O') {
                lab = "OK";
            } else if (keys[i][0] == 'C') {
                lab = hub_tr("清除", "Clear");
            }
            mk_lbl(k, lab, p->t1);
            lv_obj_center(lv_obj_get_child(k, 0));
        }
    }

    (void)go_home_cb;
    (void)go_scenes_cb;
    (void)refresh_cb;
}

