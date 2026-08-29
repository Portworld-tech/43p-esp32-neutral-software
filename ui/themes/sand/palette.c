/** Sand — palette */
#include "theme_local.h"
#include "hub_ui.h"
#include "hub_theme.h"
#include "hub_font.h"
#include "hub_model.h"
#include "hub_icons.h"
#include "app_ui.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

static const hub_palette_t s_pal = {
    .bg_deep = LV_COLOR_MAKE(244, 239, 230),
    .bg_base = LV_COLOR_MAKE(235, 228, 216),
    .bg_card = LV_COLOR_MAKE(255, 255, 255),
    .bg_card2 = LV_COLOR_MAKE(247, 242, 234),
    .bg_elev = LV_COLOR_MAKE(232, 224, 212),
    .line = LV_COLOR_MAKE(216, 206, 192),
    .t1 = LV_COLOR_MAKE(42, 36, 28),
    .t2 = LV_COLOR_MAKE(92, 83, 72),
    .t3 = LV_COLOR_MAKE(138, 127, 112),
    .t4 = LV_COLOR_MAKE(176, 165, 148),
    .accent = LV_COLOR_MAKE(196, 92, 38),
    .accent2 = LV_COLOR_MAKE(143, 63, 20),
    .on = LV_COLOR_MAKE(61, 139, 90),
    .warm = LV_COLOR_MAKE(196, 92, 38),
    .heat = LV_COLOR_MAKE(217, 119, 6),
    .cool = LV_COLOR_MAKE(42, 122, 140),
    .violet = LV_COLOR_MAKE(124, 107, 176),
    .alert = LV_COLOR_MAKE(194, 59, 59),
    .ink_on = LV_COLOR_MAKE(255, 248, 240),
    .radius = 16,
    .radius_sm = 10,
    .name = "sand",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
