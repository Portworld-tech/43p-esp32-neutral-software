#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HUB_ROOM_COUNT 4
#define HUB_PROTO_COUNT 4
#define HUB_SCENE_ID_MAX 16
#define HUB_WIDGET_MAX 12
#define HUB_WIDGET_NAME_MAX 16
#define HUB_TOAST_MAX 48
#define HUB_SCHED_COUNT 3
#define HUB_SCHED_TITLE_MAX 20
#define HUB_SCHED_META_MAX 24

typedef enum {
    HUB_W_ONOFF = 0,
    HUB_W_DIMMER,
    HUB_W_CURTAIN,
    HUB_W_SHUTTER,
    HUB_W_CLIM,
    HUB_W_THERMO,
    HUB_W_FAN,
    HUB_W_PLUG,
    HUB_W_TYPE_COUNT
} hub_wtype_t;

typedef struct {
    hub_wtype_t type;
    bool enabled;
    bool on;
    int level; /* dimmer%/curtain%/shutter%/setpoint */
    char name[HUB_WIDGET_NAME_MAX];
    char name_en[HUB_WIDGET_NAME_MAX];
} hub_widget_t;

/** Legacy flat device view (Ink matrix / home status). Synced from widgets. */
typedef struct {
    bool ceiling;
    int strip;
    int curtain;
    int shutter;
    bool ac_on;
    int ac_sp;
    bool heat_on;
    int heat_sp;
    bool fan_on;
    bool pump_ok;
    bool valve_ok;
    bool plug_on;
} hub_room_dev_t;

typedef struct {
    const char *id;
    const char *name;
    bool ok;
    int health;
} hub_proto_t;

/** FE TplSchedule row */
typedef struct {
    const char *id;
    const char *scene_id;
    char title[HUB_SCHED_TITLE_MAX];
    char time[8];
    char days[HUB_SCHED_META_MAX];
    char action[HUB_SCHED_META_MAX];
    bool on;
} hub_sched_t;

/** FE settings (TplSettings) */
typedef struct {
    bool night_mode;
    bool lang_zh; /* true = 简体中文 */
    bool click_sound;
} hub_settings_t;

typedef struct {
    char active_scene[HUB_SCENE_ID_MAX];
    bool armed;
    float power_kw;
    float indoor_c;
    int rh;
    int online_pts;
    int room_idx;
    int room_subpage; /* AdaptiveGrid page within room */
    hub_room_dev_t rooms[HUB_ROOM_COUNT];
    hub_widget_t widgets[HUB_ROOM_COUNT][HUB_WIDGET_MAX];
    int widget_count[HUB_ROOM_COUNT];
    hub_proto_t protos[HUB_PROTO_COUNT];
    hub_sched_t schedules[HUB_SCHED_COUNT];
    hub_settings_t settings;
    char toast[HUB_TOAST_MAX];
    bool toast_pending;
} hub_model_t;

void hub_model_init(void);
hub_model_t *hub_model(void);

void hub_model_apply_scene(const char *scene_id);
void hub_model_toggle_proto(const char *id);
void hub_model_set_armed(bool armed);
void hub_model_toggle_zone(int cell);
void hub_model_set_room(int idx);
bool hub_model_room_active(int idx);
const char *hub_model_room_name(int idx);
int hub_model_ok_count(void);
void hub_model_step_ac(int delta);
void hub_model_step_curtain(int delta);
void hub_model_step_strip(int delta);
void hub_model_toggle_ceiling(void);

/** Scene title for active_scene id (回家/Home, …). */
const char *hub_model_scene_label(const char *scene_id);
/** Scene subtitle (灯 + 空调 / Lights + AC, …). */
const char *hub_model_scene_sub(const char *scene_id);

/** Sync legacy rooms[] from widgets for home/Ink bindings. */
void hub_model_sync_legacy(void);

int hub_model_enabled_widget_count(int room_idx);
hub_widget_t *hub_model_widget_at(int room_idx, int enabled_index);
hub_widget_t *hub_model_widget_by_slot(int room_idx, int slot);
void hub_model_toggle_widget(int room_idx, int slot);
void hub_model_step_widget(int room_idx, int slot, int delta);
void hub_model_set_widget_level(int room_idx, int slot, int level);
void hub_model_set_widget_enabled(int room_idx, int slot, bool enabled);
bool hub_model_add_widget(int room_idx, hub_wtype_t type);
void hub_model_remove_widget(int room_idx, int slot);
const char *hub_model_wtype_label(hub_wtype_t type);
/** Widget display name respecting settings.lang_zh. */
const char *hub_model_widget_label(const hub_widget_t *w);

void hub_model_toast(const char *msg);
bool hub_model_take_toast(char *out, size_t out_sz);

void hub_model_toggle_schedule(int idx);
void hub_model_run_schedule(int idx);
int hub_model_sched_on_count(void);
/** Bilingual schedule title / days / action for display. */
const char *hub_model_sched_title(int idx);
const char *hub_model_sched_days(int idx);
const char *hub_model_sched_action(int idx);
void hub_model_set_night_mode(bool on);
void hub_model_set_click_sound(bool on);
void hub_model_toggle_lang(void);

#ifdef __cplusplus
}
#endif
