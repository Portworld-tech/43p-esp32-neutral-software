#pragma once

#include "lvgl.h"
#include "hub_theme.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HUB_ROUTE_HOME = 0,
    HUB_ROUTE_ROOM,
    HUB_ROUTE_SCENES,
    HUB_ROUTE_ENERGY,
    HUB_ROUTE_GATEWAY,
    HUB_ROUTE_SETTINGS,
    HUB_ROUTE_NETWORK,
    HUB_ROUTE_SECURITY,
    HUB_ROUTE_SCHEDULE,
    HUB_ROUTE_HVAC,
    HUB_ROUTE_POINTS,
    HUB_ROUTE_ROOM_EDIT,
    HUB_ROUTE_APPLIANCES,
    HUB_ROUTE_STANDBY,
    HUB_ROUTE_COUNT
} hub_route_t;

void hub_ui_init(void);
void hub_ui_go(hub_route_t route);
/** Rebuild current route (after model mutation). */
void hub_ui_refresh(void);
hub_route_t hub_ui_route(void);
void hub_nav_attach(lv_obj_t *root);

/**
 * Per-theme pack owns ALL pages (no shared hub_pages).
 * Implemented under ui/themes/<id>/ as a multi-file pack per theme.
 */
void hub_theme_build(lv_obj_t *parent, hub_route_t route);

lv_obj_t *hub_shell_create(lv_obj_t *parent, const char *title, bool show_back);
lv_obj_t *hub_shell_body(lv_obj_t *shell);
void hub_shell_set_dots(lv_obj_t *shell, int count, int active);

#ifdef __cplusplus
}
#endif
