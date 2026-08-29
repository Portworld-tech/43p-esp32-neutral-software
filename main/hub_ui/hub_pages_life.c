#include "hub_pages_life.h"

#include "hub_ui.h"
#include "hub_theme.h"
#include "hub_font.h"
#include "hub_model.h"
#include "hub_icons.h"
#include "hub_i18n.h"
#include "hub_device_ui.h"
#include "hub_standby.h"
#include "hub_pages_common.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void go_cb(lv_event_t *e)
{
    hub_ui_go((hub_route_t)(uintptr_t)lv_event_get_user_data(e));
}

static lv_obj_t *lbl(lv_obj_t *par, const char *txt, lv_color_t c, const lv_font_t *f)
{
    lv_obj_t *l = lv_label_create(par);
    lv_label_set_text(l, txt ? txt : "");
    hub_style_label(l, c, f);
    return l;
}

static void night_cb(lv_event_t *e)
{
    (void)e;
    hub_model_t *m = hub_model();
    hub_model_set_night_mode(!m->settings.night_mode);
    hub_device_brightness_apply_effective();
    hub_ui_refresh();
}

static void standby_en_cb(lv_event_t *e)
{
    (void)e;
    hub_standby_set_enabled(!hub_standby_enabled());
    hub_ui_refresh();
}

static void standby_tmo_cb(lv_event_t *e)
{
    lv_obj_t *dd = lv_event_get_target(e);
    hub_standby_set_idle_index((int)lv_dropdown_get_selected(dd));
    hub_model_toast(hub_standby_idle_label());
}

static void standby_preview_cb(lv_event_t *e)
{
    (void)e;
    hub_ui_go(HUB_ROUTE_STANDBY);
}

static void sound_cb(lv_event_t *e)
{
    (void)e;
    hub_model_t *m = hub_model();
    hub_model_set_click_sound(!m->settings.click_sound);
    hub_ui_refresh();
}

static void lang_cb(lv_event_t *e)
{
    (void)e;
    hub_model_toggle_lang();
    hub_ui_refresh();
}

static void sched_toggle_cb(lv_event_t *e)
{
    int idx = (int)(uintptr_t)lv_event_get_user_data(e);
    hub_model_toggle_schedule(idx);
    hub_ui_refresh();
}

static void sched_run_cb(lv_event_t *e)
{
    int idx = (int)(uintptr_t)lv_event_get_user_data(e);
    hub_model_run_schedule(idx);
    hub_ui_refresh();
}

static void heat_step_cb(lv_event_t *e)
{
    int delta = (int)(intptr_t)lv_event_get_user_data(e);
    hub_model_t *m = hub_model();
    hub_room_dev_t *d = &m->rooms[m->room_idx];
    int sp = d->heat_sp + delta;
    if (sp < 16) {
        sp = 16;
    }
    if (sp > 30) {
        sp = 30;
    }
    d->heat_sp = sp;
    for (int i = 0; i < m->widget_count[m->room_idx]; i++) {
        hub_widget_t *w = &m->widgets[m->room_idx][i];
        if (w->type == HUB_W_THERMO && w->enabled) {
            w->level = sp;
            break;
        }
    }
    hub_ui_refresh();
}

static void heat_toggle_cb(lv_event_t *e)
{
    (void)e;
    hub_model_t *m = hub_model();
    hub_room_dev_t *d = &m->rooms[m->room_idx];
    d->heat_on = !d->heat_on;
    for (int i = 0; i < m->widget_count[m->room_idx]; i++) {
        hub_widget_t *w = &m->widgets[m->room_idx][i];
        if (w->type == HUB_W_THERMO && w->enabled) {
            w->on = d->heat_on;
            break;
        }
    }
    hub_model_toast(d->heat_on ? hub_tr("地暖开启", "Heat on") : hub_tr("地暖关闭", "Heat off"));
    hub_ui_refresh();
}

static void ac_toggle_cb(lv_event_t *e)
{
    (void)e;
    hub_model_t *m = hub_model();
    hub_room_dev_t *d = &m->rooms[m->room_idx];
    d->ac_on = !d->ac_on;
    for (int i = 0; i < m->widget_count[m->room_idx]; i++) {
        hub_widget_t *w = &m->widgets[m->room_idx][i];
        if (w->type == HUB_W_CLIM && w->enabled) {
            w->on = d->ac_on;
            break;
        }
    }
    hub_model_toast(d->ac_on ? hub_tr("空调开启", "AC on") : hub_tr("空调关闭", "AC off"));
    hub_ui_refresh();
}

static void ac_step_cb(lv_event_t *e)
{
    int delta = (int)(intptr_t)lv_event_get_user_data(e);
    hub_model_step_ac(delta);
    hub_ui_refresh();
}

/** FE sf-switch — radius follows theme palette. */
static lv_obj_t *mk_switch(lv_obj_t *par, bool on, lv_event_cb_t cb, void *ud)
{
    const hub_palette_t *p = hub_palette();
    lv_coord_t r = p->radius;
    lv_obj_t *sw = lv_btn_create(par);
    lv_obj_remove_style_all(sw);
    lv_obj_set_size(sw, 44, 24);
    lv_obj_set_style_radius(sw, r, 0);
    lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(sw, on ? p->accent : lv_color_mix(p->t1, p->bg_deep, LV_OPA_20), 0);
    lv_obj_add_event_cb(sw, cb, LV_EVENT_CLICKED, ud);
    lv_obj_t *kn = lv_obj_create(sw);
    lv_obj_remove_style_all(kn);
    lv_obj_set_size(kn, 18, 18);
    lv_obj_set_style_radius(kn, r > 0 ? (r > 9 ? 9 : r) : 0, 0);
    lv_obj_set_style_bg_opa(kn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(kn, p->ink_on, 0);
    lv_obj_align(kn, on ? LV_ALIGN_RIGHT_MID : LV_ALIGN_LEFT_MID, on ? -3 : 3, 0);
    lv_obj_clear_flag(kn, LV_OBJ_FLAG_CLICKABLE);
    return sw;
}

static lv_obj_t *mk_setting_row(lv_obj_t *body, hub_ico_t ico, const char *title, const char *sub)
{
    const hub_palette_t *p = hub_palette();
    lv_obj_t *row = lv_obj_create(body);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, 56);
    lv_obj_set_style_radius(row, p->radius, 0);
    lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(row, 1, 0);
    lv_obj_set_style_border_color(row, p->line, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    hub_ico_badge(row, ico, p->accent, 36, 18);
    lv_obj_align(lv_obj_get_child(row, 0), LV_ALIGN_LEFT_MID, 4, 0);
    lv_obj_t *t = lv_label_create(row);
    lv_label_set_text(t, title);
    hub_style_label(t, p->t1, hub_font());
    lv_obj_align(t, LV_ALIGN_LEFT_MID, 48, -8);
    lv_obj_t *s = lv_label_create(row);
    lv_label_set_text(s, sub);
    hub_style_label(s, p->t3, hub_font());
    lv_obj_align(s, LV_ALIGN_LEFT_MID, 48, 10);
    return row;
}

static lv_obj_t *mk_nav_row(lv_obj_t *body, hub_ico_t ico, const char *title, const char *sub,
                            hub_route_t route)
{
    const hub_palette_t *p = hub_palette();
    lv_obj_t *row = lv_btn_create(body);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, 56);
    lv_obj_set_style_radius(row, p->radius, 0);
    lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(row, 1, 0);
    lv_obj_set_style_border_color(row, p->line, 0);
    hub_ico_badge(row, ico, p->accent, 36, 18);
    lv_obj_align(lv_obj_get_child(row, 0), LV_ALIGN_LEFT_MID, 4, 0);
    lv_obj_t *t = lv_label_create(row);
    lv_label_set_text(t, title);
    hub_style_label(t, p->t1, hub_font());
    lv_obj_align(t, LV_ALIGN_LEFT_MID, 48, -8);
    lv_obj_t *s = lv_label_create(row);
    lv_label_set_text(s, sub);
    hub_style_label(s, p->t3, hub_font());
    lv_obj_align(s, LV_ALIGN_LEFT_MID, 48, 10);
    hub_ico_add(row, HUB_ICO_CHEVRON, p->t4, 16);
    lv_obj_align(lv_obj_get_child(row, -1), LV_ALIGN_RIGHT_MID, -4, 0);
    lv_obj_add_event_cb(row, go_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)route);
    return row;
}

void hub_build_settings(lv_obj_t *parent)
{
    const hub_palette_t *p = hub_palette();
    hub_model_t *m = hub_model();
    bool zh = m->settings.lang_zh;
    hub_wifi_sync_model();
    lv_obj_t *shell = hub_shell_create(parent, hub_tr("设置", "Settings"), true);
    lv_obj_t *body = hub_shell_body(shell);

    lbl(body, hub_tr("显示", "Display"), p->t3, hub_font());
    hub_build_brightness_block(body);

    {
        char sub[40];
        snprintf(sub, sizeof(sub), "%s",
                 m->settings.night_mode
                     ? hub_tr("已开启 · 降亮", "On · dimmed")
                     : hub_tr("跟随白天", "Follow day"));
        lv_obj_t *row = mk_setting_row(body, HUB_ICO_MOON, hub_tr("夜间模式", "Night mode"), sub);
        mk_switch(row, m->settings.night_mode, night_cb, NULL);
        lv_obj_align(lv_obj_get_child(row, -1), LV_ALIGN_RIGHT_MID, -4, 0);
    }

    {
        char sub[48];
        snprintf(sub, sizeof(sub), "%s · %s",
                 hub_standby_enabled() ? hub_tr("已开启", "On") : hub_tr("已关闭", "Off"),
                 hub_standby_idle_label());
        lv_obj_t *row = mk_setting_row(body, HUB_ICO_CLOCK,
                                       hub_tr("低功耗待机", "Low-power standby"), sub);
        mk_switch(row, hub_standby_enabled(), standby_en_cb, NULL);
        lv_obj_align(lv_obj_get_child(row, -1), LV_ALIGN_RIGHT_MID, -4, 0);
    }

    if (hub_standby_enabled()) {
        lbl(body, hub_tr("进入待机时长", "Idle before standby"), p->t3, hub_font());
        lv_obj_t *dd = lv_dropdown_create(body);
        lv_obj_set_width(dd, LV_PCT(100));
        hub_style_dropdown(dd);
        lv_obj_set_style_radius(dd, p->radius, 0);
        lv_dropdown_set_options(dd, zh
            ? "15 秒\n30 秒\n1 分钟\n2 分钟\n5 分钟\n10 分钟"
            : "15 sec\n30 sec\n1 min\n2 min\n5 min\n10 min");
        lv_dropdown_set_selected(dd, (uint16_t)hub_standby_idle_index());
        lv_obj_add_event_cb(dd, standby_tmo_cb, LV_EVENT_VALUE_CHANGED, NULL);

        lv_obj_t *prev = lv_btn_create(body);
        lv_obj_remove_style_all(prev);
        lv_obj_set_width(prev, LV_PCT(100));
        lv_obj_set_height(prev, 40);
        lv_obj_set_style_radius(prev, p->radius, 0);
        lv_obj_set_style_border_width(prev, 1, 0);
        lv_obj_set_style_border_color(prev, p->line, 0);
        lv_obj_add_event_cb(prev, standby_preview_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_t *pl = lv_label_create(prev);
        lv_label_set_text(pl, hub_tr("预览待机界面 →", "Preview standby →"));
        hub_style_label(pl, p->accent, hub_font());
        lv_obj_center(pl);
    }

    lbl(body, hub_tr("系统", "System"), p->t3, hub_font());

    {
        lv_obj_t *row = mk_setting_row(body, HUB_ICO_INFO, hub_tr("界面语言", "Language"),
                                       zh ? "简体中文" : "English");
        lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(row, lang_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_t *r = lv_label_create(row);
        lv_label_set_text(r, zh ? "ZH" : "EN");
        hub_style_label(r, p->accent, hub_font());
        lv_obj_align(r, LV_ALIGN_RIGHT_MID, -8, 0);
    }

    {
        const char *net = m->protos[3].ok
                              ? hub_tr("Wi-Fi 已连接", "Wi-Fi connected")
                              : hub_tr("Wi-Fi 关闭", "Wi-Fi off");
        mk_nav_row(body, HUB_ICO_WIFI, hub_tr("网络与配网", "Network"), net, HUB_ROUTE_NETWORK);
    }

    {
        char sub[40];
        snprintf(sub, sizeof(sub), zh ? "%d 条启用" : "%d enabled", hub_model_sched_on_count());
        mk_nav_row(body, HUB_ICO_CLOCK, hub_tr("定时与日程", "Schedule"), sub, HUB_ROUTE_SCHEDULE);
    }

    mk_nav_row(body, HUB_ICO_HOME, hub_tr("房间控件", "Room widgets"),
               hub_tr("自定义家居控件布局", "Customize layout"), HUB_ROUTE_ROOM_EDIT);

    {
        char sub[40];
        snprintf(sub, sizeof(sub), zh ? "%d/%d 正常" : "%d/%d OK", hub_model_ok_count(), HUB_PROTO_COUNT);
        mk_nav_row(body, HUB_ICO_BUS, hub_tr("总线与点表", "Bus & points"), sub, HUB_ROUTE_GATEWAY);
    }

    {
        lv_obj_t *row = mk_setting_row(body, HUB_ICO_POWER, hub_tr("触控音", "Click sound"),
                                       m->settings.click_sound ? hub_tr("开启", "On")
                                                               : hub_tr("静音", "Mute"));
        mk_switch(row, m->settings.click_sound, sound_cb, NULL);
        lv_obj_align(lv_obj_get_child(row, -1), LV_ALIGN_RIGHT_MID, -4, 0);
    }

    {
        char foot[48];
        snprintf(foot, sizeof(foot), "Hub UI Kit · %s · v0.3", p->name ? p->name : "hub");
        lbl(body, foot, p->t4, hub_font());
    }
}

void hub_build_schedule(lv_obj_t *parent)
{
    const hub_palette_t *p = hub_palette();
    hub_model_t *m = hub_model();
    lv_obj_t *shell = hub_shell_create(parent, hub_tr("日程", "Schedule"), true);
    lv_obj_t *body = hub_shell_body(shell);

    lbl(body, hub_tr("开关=启用 · 点「执行」立即跑情景", "Toggle=enable · Run fires scene"),
        p->t3, hub_font());

    for (int i = 0; i < HUB_SCHED_COUNT; i++) {
        hub_sched_t *s = &m->schedules[i];
        lv_obj_t *card = lv_obj_create(body);
        lv_obj_remove_style_all(card);
        lv_obj_set_width(card, LV_PCT(100));
        lv_obj_set_height(card, 88);
        lv_obj_set_style_radius(card, p->radius, 0);
        lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(card, p->bg_card, 0);
        lv_obj_set_style_border_width(card, 1, 0);
        lv_obj_set_style_border_color(card, p->line, 0);
        lv_obj_set_style_pad_all(card, 12, 0);
        lv_obj_set_style_pad_left(card, 16, 0);
        theme_card_chrome(card, s->on);

        if (s->on) {
            lv_obj_t *bar = lv_obj_create(card);
            lv_obj_remove_style_all(bar);
            lv_obj_set_size(bar, 3, LV_PCT(70));
            lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(bar, p->accent, 0);
            lv_obj_clear_flag(bar, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_align(bar, LV_ALIGN_LEFT_MID, -12, 0);
        }

        lv_obj_t *tm = lv_label_create(card);
        lv_label_set_text(tm, s->time);
        hub_style_label(tm, p->t1, hub_font_clock());
        lv_obj_align(tm, LV_ALIGN_TOP_LEFT, 4, 0);

        lv_obj_t *st = lv_label_create(card);
        lv_label_set_text(st, s->on ? "ON" : "OFF");
        hub_style_label(st, s->on ? p->on : p->t4, hub_font());
        lv_obj_align(st, LV_ALIGN_TOP_LEFT, 100, 8);

        lv_obj_t *ti = lv_label_create(card);
        lv_label_set_text(ti, hub_model_sched_title(i));
        hub_style_label(ti, p->t1, hub_font());
        lv_obj_align(ti, LV_ALIGN_BOTTOM_LEFT, 4, -22);

        char meta[48];
        snprintf(meta, sizeof(meta), "%s · %s", hub_model_sched_days(i), hub_model_sched_action(i));
        lv_obj_t *me = lv_label_create(card);
        lv_label_set_text(me, meta);
        hub_style_label(me, p->t3, hub_font());
        lv_obj_align(me, LV_ALIGN_BOTTOM_LEFT, 4, -4);

        lv_obj_t *run = lv_btn_create(card);
        lv_obj_remove_style_all(run);
        lv_obj_set_size(run, 52, 32);
        lv_obj_set_style_radius(run, p->radius, 0);
        lv_obj_set_style_bg_opa(run, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(run, s->on ? p->accent : p->bg_card2, 0);
        lv_obj_set_style_border_width(run, 1, 0);
        lv_obj_set_style_border_color(run, p->line, 0);
        lv_obj_align(run, LV_ALIGN_RIGHT_MID, -56, 0);
        if (s->on) {
            lv_obj_add_event_cb(run, sched_run_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
        } else {
            lv_obj_set_style_opa(run, LV_OPA_50, 0);
        }
        lv_obj_t *rl = lv_label_create(run);
        lv_label_set_text(rl, hub_tr("执行", "Run"));
        hub_style_label(rl, s->on ? p->ink_on : p->t4, hub_font());
        lv_obj_center(rl);

        mk_switch(card, s->on, sched_toggle_cb, (void *)(uintptr_t)i);
        lv_obj_align(lv_obj_get_child(card, -1), LV_ALIGN_RIGHT_MID, -4, 0);
    }

    lv_obj_t *cta = lv_btn_create(body);
    lv_obj_remove_style_all(cta);
    lv_obj_set_width(cta, LV_PCT(100));
    lv_obj_set_height(cta, 44);
    lv_obj_set_style_radius(cta, p->radius, 0);
    lv_obj_set_style_bg_opa(cta, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cta, p->t1, 0);
    lv_obj_add_event_cb(cta, go_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)HUB_ROUTE_SCENES);
    lv_obj_t *cl = lv_label_create(cta);
    lv_label_set_text(cl, hub_tr("前往情景中心 →", "Scenes →"));
    hub_style_label(cl, p->ink_on, hub_font());
    lv_obj_center(cl);
}

void hub_build_hvac(lv_obj_t *parent)
{
    const hub_palette_t *p = hub_palette();
    hub_model_t *m = hub_model();
    hub_room_dev_t *d = &m->rooms[m->room_idx];

    lv_obj_t *shell = hub_shell_create(parent, hub_tr("温控", "Climate"), true);
    lv_obj_t *body = hub_shell_body(shell);
    hub_shell_set_dots(shell, 3, 0);

    lv_obj_t *ac = lv_obj_create(body);
    lv_obj_remove_style_all(ac);
    hub_apply_card(ac, d->ac_on);
    theme_card_chrome(ac, d->ac_on);
    lv_obj_set_width(ac, LV_PCT(100));
    lv_obj_set_height(ac, 160);
    lv_obj_set_style_radius(ac, p->radius, 0);

    hub_ico_add(ac, HUB_ICO_SNOW, p->cool, 28);
    lv_obj_align(lv_obj_get_child(ac, 0), LV_ALIGN_TOP_LEFT, 12, 12);
    lbl(ac, "AC", p->t1, hub_font());
    lv_obj_align(lv_obj_get_child(ac, 1), LV_ALIGN_TOP_LEFT, 48, 16);
    lbl(ac, hub_model_room_name(m->room_idx), p->t3, hub_font());
    lv_obj_align(lv_obj_get_child(ac, 2), LV_ALIGN_TOP_LEFT, 48, 34);

    lv_obj_t *acpwr = lv_btn_create(ac);
    lv_obj_remove_style_all(acpwr);
    lv_obj_set_size(acpwr, 36, 36);
    lv_obj_set_style_radius(acpwr, p->radius, 0);
    lv_obj_set_style_bg_opa(acpwr, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(acpwr, d->ac_on ? p->accent : p->bg_card2, 0);
    lv_obj_align(acpwr, LV_ALIGN_TOP_RIGHT, -12, 12);
    lv_obj_add_event_cb(acpwr, ac_toggle_cb, LV_EVENT_CLICKED, NULL);
    hub_ico_add(acpwr, HUB_ICO_POWER, d->ac_on ? p->ink_on : p->t2, 14);
    lv_obj_center(lv_obj_get_child(acpwr, 0));

    char sp[16];
    snprintf(sp, sizeof(sp), d->ac_on ? "%d°" : "--", d->ac_sp);
    lv_obj_t *sv = lv_label_create(ac);
    lv_label_set_text(sv, sp);
    hub_style_label(sv, d->ac_on ? p->cool : p->t4, hub_font_clock_lg());
    lv_obj_align(sv, LV_ALIGN_LEFT_MID, 12, 10);

    char amb[24];
    snprintf(amb, sizeof(amb), hub_tr("室内 %.1f°", "Indoor %.1f°"), (double)m->indoor_c);
    lbl(ac, amb, p->t3, hub_font());
    lv_obj_align(lv_obj_get_child(ac, -1), LV_ALIGN_BOTTOM_LEFT, 12, -14);

    lv_obj_t *am = lv_btn_create(ac);
    lv_obj_remove_style_all(am);
    lv_obj_set_size(am, 48, 40);
    lv_obj_set_style_radius(am, p->radius, 0);
    lv_obj_set_style_border_width(am, 1, 0);
    lv_obj_set_style_border_color(am, p->line, 0);
    lv_obj_align(am, LV_ALIGN_BOTTOM_RIGHT, -68, -12);
    lv_obj_add_event_cb(am, ac_step_cb, LV_EVENT_CLICKED, (void *)(intptr_t)-1);
    hub_ico_add(am, HUB_ICO_MINUS, p->t1, 18);
    lv_obj_center(lv_obj_get_child(am, 0));

    lv_obj_t *ap = lv_btn_create(ac);
    lv_obj_remove_style_all(ap);
    lv_obj_set_size(ap, 48, 40);
    lv_obj_set_style_radius(ap, p->radius, 0);
    lv_obj_set_style_border_width(ap, 1, 0);
    lv_obj_set_style_border_color(ap, p->line, 0);
    lv_obj_align(ap, LV_ALIGN_BOTTOM_RIGHT, -12, -12);
    lv_obj_add_event_cb(ap, ac_step_cb, LV_EVENT_CLICKED, (void *)(intptr_t)1);
    hub_ico_add(ap, HUB_ICO_PLUS, p->t1, 18);
    lv_obj_center(lv_obj_get_child(ap, 0));

    lv_obj_t *ht = lv_obj_create(body);
    lv_obj_remove_style_all(ht);
    hub_apply_card(ht, d->heat_on);
    theme_card_chrome(ht, d->heat_on);
    lv_obj_set_width(ht, LV_PCT(100));
    lv_obj_set_height(ht, 140);
    lv_obj_set_style_radius(ht, p->radius, 0);

    hub_ico_add(ht, HUB_ICO_GAUGE, p->heat, 28);
    lv_obj_align(lv_obj_get_child(ht, 0), LV_ALIGN_TOP_LEFT, 12, 12);
    lbl(ht, hub_tr("地暖", "Floor heat"), p->t1, hub_font());
    lv_obj_align(lv_obj_get_child(ht, 1), LV_ALIGN_TOP_LEFT, 48, 16);

    lv_obj_t *hp = lv_btn_create(ht);
    lv_obj_remove_style_all(hp);
    lv_obj_set_size(hp, 36, 36);
    lv_obj_set_style_radius(hp, p->radius, 0);
    lv_obj_set_style_bg_opa(hp, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(hp, d->heat_on ? p->accent : p->bg_card2, 0);
    lv_obj_align(hp, LV_ALIGN_TOP_RIGHT, -12, 12);
    lv_obj_add_event_cb(hp, heat_toggle_cb, LV_EVENT_CLICKED, NULL);
    hub_ico_add(hp, HUB_ICO_POWER, d->heat_on ? p->ink_on : p->t2, 14);
    lv_obj_center(lv_obj_get_child(hp, 0));

    char hsp[16];
    snprintf(hsp, sizeof(hsp), d->heat_on ? "%d°" : "--", d->heat_sp);
    lv_obj_t *hv = lv_label_create(ht);
    lv_label_set_text(hv, hsp);
    hub_style_label(hv, d->heat_on ? p->heat : p->t4, hub_font_clock_lg());
    lv_obj_align(hv, LV_ALIGN_LEFT_MID, 12, 8);

    lv_obj_t *hm = lv_btn_create(ht);
    lv_obj_remove_style_all(hm);
    lv_obj_set_size(hm, 48, 40);
    lv_obj_set_style_radius(hm, p->radius, 0);
    lv_obj_set_style_border_width(hm, 1, 0);
    lv_obj_set_style_border_color(hm, p->line, 0);
    lv_obj_align(hm, LV_ALIGN_BOTTOM_RIGHT, -68, -12);
    lv_obj_add_event_cb(hm, heat_step_cb, LV_EVENT_CLICKED, (void *)(intptr_t)-1);
    hub_ico_add(hm, HUB_ICO_MINUS, p->t1, 18);
    lv_obj_center(lv_obj_get_child(hm, 0));

    lv_obj_t *hpp = lv_btn_create(ht);
    lv_obj_remove_style_all(hpp);
    lv_obj_set_size(hpp, 48, 40);
    lv_obj_set_style_radius(hpp, p->radius, 0);
    lv_obj_set_style_border_width(hpp, 1, 0);
    lv_obj_set_style_border_color(hpp, p->line, 0);
    lv_obj_align(hpp, LV_ALIGN_BOTTOM_RIGHT, -12, -12);
    lv_obj_add_event_cb(hpp, heat_step_cb, LV_EVENT_CLICKED, (void *)(intptr_t)1);
    hub_ico_add(hpp, HUB_ICO_PLUS, p->t1, 18);
    lv_obj_center(lv_obj_get_child(hpp, 0));
}
