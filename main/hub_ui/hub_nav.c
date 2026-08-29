#include "hub_ui.h"
#include "hub_model.h"
#include "hub_device_ui.h"

typedef struct {
    lv_coord_t x;
    lv_coord_t y;
    bool active;
} swipe_state_t;

static swipe_state_t s_swipe;

static const hub_route_t s_pager[] = {
    HUB_ROUTE_HOME,
    HUB_ROUTE_ROOM,
    HUB_ROUTE_SCENES,
    HUB_ROUTE_ENERGY,
};
#define PAGER_N (sizeof(s_pager) / sizeof(s_pager[0]))
#define ROOM_PAGE_SIZE 6

static int pager_index(hub_route_t r)
{
    for (int i = 0; i < (int)PAGER_N; i++) {
        if (s_pager[i] == r) {
            return i;
        }
    }
    return -1;
}

static void do_swipe(bool to_next)
{
    hub_route_t cur = hub_ui_route();

    /* Room AdaptiveGrid: swipe across subpages before leaving room route */
    if (cur == HUB_ROUTE_ROOM) {
        hub_model_t *m = hub_model();
        int enabled = hub_model_enabled_widget_count(m->room_idx);
        int pages = enabled > 0 ? (enabled + ROOM_PAGE_SIZE - 1) / ROOM_PAGE_SIZE : 1;
        if (pages > 1) {
            if (to_next) {
                if (m->room_subpage < pages - 1) {
                    m->room_subpage++;
                    hub_ui_refresh();
                    return;
                }
            } else if (m->room_subpage > 0) {
                m->room_subpage--;
                hub_ui_refresh();
                return;
            }
        }
    }

    int idx = pager_index(cur);
    if (idx < 0) {
        return;
    }
    if (to_next) {
        hub_ui_go(s_pager[(idx + 1) % (int)PAGER_N]);
    } else {
        hub_ui_go(s_pager[(idx + (int)PAGER_N - 1) % (int)PAGER_N]);
    }
}

static void on_gesture(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_GESTURE) {
        return;
    }
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_LEFT) {
        do_swipe(true);
    } else if (dir == LV_DIR_RIGHT) {
        do_swipe(false);
    }
}

/* Fallback: press/release on root (works when gesture not generated). */
static void on_pressed(lv_event_t *e)
{
    lv_indev_t *indev = lv_indev_get_act();
    if (!indev) {
        return;
    }
    lv_point_t pt;
    lv_indev_get_point(indev, &pt);
    s_swipe.x = pt.x;
    s_swipe.y = pt.y;
    s_swipe.active = true;
    (void)e;
}

static void on_released(lv_event_t *e)
{
    (void)e;
    if (!s_swipe.active) {
        return;
    }
    s_swipe.active = false;
    lv_indev_t *indev = lv_indev_get_act();
    if (!indev) {
        return;
    }
    lv_point_t pt;
    lv_indev_get_point(indev, &pt);
    lv_coord_t dx = pt.x - s_swipe.x;
    lv_coord_t dy = pt.y - s_swipe.y;
    if (LV_ABS(dx) < 72) {
        return;
    }
    if (LV_ABS(dx) < LV_ABS(dy) * 130 / 100) {
        return;
    }
    do_swipe(dx < 0);
}

void hub_nav_attach(lv_obj_t *root)
{
    lv_obj_add_event_cb(root, on_gesture, LV_EVENT_GESTURE, NULL);
    lv_obj_add_event_cb(root, on_pressed, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(root, on_released, LV_EVENT_RELEASED, NULL);
    /* Root must NOT have GESTURE_BUBBLE — it is the sink for bubbled gestures. */
    lv_obj_clear_flag(root, LV_OBJ_FLAG_GESTURE_BUBBLE);
}
