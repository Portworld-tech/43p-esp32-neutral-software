#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** AdaptiveGrid room page (FE HubRoomPage parity). */
void hub_build_room_page(lv_obj_t *parent);

/** Room widget editor (FE HubRoomEditPage). */
void hub_build_room_edit(lv_obj_t *parent);

/** Appliance switches (FE TplApplianceGrid / Ink PUMP·VALVE target). */
void hub_build_appliances(lv_obj_t *parent);

/** Security with PIN-gated disarm. */
void hub_build_security(lv_obj_t *parent);

/** Show pending toast if any (call after page build). */
void hub_toast_present(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif
