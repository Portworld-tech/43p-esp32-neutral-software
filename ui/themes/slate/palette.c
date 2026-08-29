/** Slate — palette */
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
    .bg_deep = LV_COLOR_MAKE(15, 18, 20),
    .bg_base = LV_COLOR_MAKE(22, 26, 30),
    .bg_card = LV_COLOR_MAKE(28, 34, 40),
    .bg_card2 = LV_COLOR_MAKE(37, 44, 52),
    .bg_elev = LV_COLOR_MAKE(46, 54, 64),
    .line = LV_COLOR_MAKE(40, 44, 48),
    .t1 = LV_COLOR_MAKE(243, 245, 247),
    .t2 = LV_COLOR_MAKE(184, 192, 200),
    .t3 = LV_COLOR_MAKE(125, 135, 146),
    .t4 = LV_COLOR_MAKE(82, 92, 102),
    .accent = LV_COLOR_MAKE(45, 212, 191),
    .accent2 = LV_COLOR_MAKE(15, 118, 110),
    .on = LV_COLOR_MAKE(52, 211, 153),
    .warm = LV_COLOR_MAKE(234, 179, 8),
    .heat = LV_COLOR_MAKE(245, 158, 11),
    .cool = LV_COLOR_MAKE(34, 211, 238),
    .violet = LV_COLOR_MAKE(129, 140, 248),
    .alert = LV_COLOR_MAKE(244, 63, 94),
    .ink_on = LV_COLOR_MAKE(4, 47, 46),
    .radius = 12,
    .radius_sm = 8,
    .name = "slate",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
