#include "ui_runtime.h"

#include "hub_model.h"
#include "hub_ui.h"

#include <string.h>

void ui_runtime_apply(void) {}
void ui_runtime_disable_btn_grow_everywhere(void) {}
void ui_runtime_disable_btn_grow_on(lv_obj_t *root) { (void)root; }
bool ui_runtime_screen_switch_busy(void) { return false; }
void ui_runtime_rgb_commit_active_screen(void) {}
void ui_runtime_rgb_commit_full_screen(void) {}
void ui_runtime_set_network_info(const char *mac, const char *ip)
{
    (void)mac;
    (void)ip;
}
void ui_runtime_step_screen3_temp(int delta) { (void)delta; }
void ui_runtime_step_screen5_temp(int delta) { (void)delta; }
void ui_runtime_fill_cloud_snapshot(ui_runtime_cloud_snapshot_t *out)
{
    if (out) {
        memset(out, 0, sizeof(*out));
    }
}
bool ui_runtime_allow_mqtt_temp_item(uint8_t item_id)
{
    (void)item_id;
    return false;
}
void ui_runtime_screen10_pick_and_home(uint8_t pick) { (void)pick; }
void ui_runtime_screen_change_by_obj(lv_obj_t *scr) { (void)scr; }

void ui_runtime_indoor_apply_climate(float temp_c, int rh_pct)
{
    hub_model_t *m = hub_model();
    if (!m) {
        return;
    }
    int rh = rh_pct;
    if (rh < 0) {
        rh = 0;
    }
    if (rh > 100) {
        rh = 100;
    }

    const int t10 = (int)(temp_c * 10.0f + (temp_c >= 0.0f ? 0.5f : -0.5f));
    const int old_t10 = (int)(m->indoor_c * 10.0f + (m->indoor_c >= 0.0f ? 0.5f : -0.5f));
    const bool changed = (t10 != old_t10) || (m->rh != rh);
    m->indoor_c = temp_c;
    m->rh = rh;
    if (changed) {
        hub_ui_refresh();
    }
}

void ui_runtime_indoor_apply_temp(int temp_c)
{
    hub_model_t *m = hub_model();
    const int rh = m ? m->rh : 0;
    ui_runtime_indoor_apply_climate((float)temp_c, rh);
}
