/** Pulse — settings / schedule / hvac / security via shared hub builders */
#include "theme_local.h"
#include "hub_pages_life.h"
#include "hub_room_ui.h"

void build_settings(lv_obj_t *parent)
{
    hub_build_settings(parent);
}

void build_security(lv_obj_t *parent)
{
    hub_build_security(parent);
}

void build_schedule(lv_obj_t *parent)
{
    hub_build_schedule(parent);
}

void build_hvac(lv_obj_t *parent)
{
    hub_build_hvac(parent);
}