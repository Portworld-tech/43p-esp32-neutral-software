#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Shared life pages (Zen-parity) — used by all hub theme packs. */
void hub_build_settings(lv_obj_t *parent);
void hub_build_schedule(lv_obj_t *parent);
void hub_build_hvac(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif
