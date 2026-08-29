/** Dusk — palette */
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
    .bg_deep = LV_COLOR_MAKE(18, 16, 26),
    .bg_base = LV_COLOR_MAKE(24, 21, 36),
    .bg_card = LV_COLOR_MAKE(33, 30, 48),
    .bg_card2 = LV_COLOR_MAKE(44, 40, 64),
    .bg_elev = LV_COLOR_MAKE(58, 53, 82),
    .line = LV_COLOR_MAKE(50, 45, 70),
    .t1 = LV_COLOR_MAKE(243, 239, 255),
    .t2 = LV_COLOR_MAKE(200, 191, 217),
    .t3 = LV_COLOR_MAKE(145, 136, 168),
    .t4 = LV_COLOR_MAKE(107, 98, 128),
    .accent = LV_COLOR_MAKE(167, 139, 250),
    .accent2 = LV_COLOR_MAKE(109, 40, 217),
    .on = LV_COLOR_MAKE(52, 211, 153),
    .warm = LV_COLOR_MAKE(240, 171, 252),
    .heat = LV_COLOR_MAKE(251, 113, 133),
    .cool = LV_COLOR_MAKE(103, 232, 249),
    .violet = LV_COLOR_MAKE(196, 181, 253),
    .alert = LV_COLOR_MAKE(251, 113, 133),
    .ink_on = LV_COLOR_MAKE(30, 16, 53),
    .radius = 18,
    .radius_sm = 12,
    .name = "dusk",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
