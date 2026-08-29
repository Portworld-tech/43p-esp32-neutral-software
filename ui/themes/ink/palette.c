/** Ink — palette */
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
    .bg_deep = LV_COLOR_MAKE(0, 0, 0),
    .bg_base = LV_COLOR_MAKE(10, 10, 10),
    .bg_card = LV_COLOR_MAKE(17, 17, 17),
    .bg_card2 = LV_COLOR_MAKE(26, 26, 26),
    .bg_elev = LV_COLOR_MAKE(36, 36, 36),
    .line = LV_COLOR_MAKE(40, 40, 40),
    .t1 = LV_COLOR_MAKE(250, 250, 250),
    .t2 = LV_COLOR_MAKE(212, 212, 212),
    .t3 = LV_COLOR_MAKE(163, 163, 163),
    .t4 = LV_COLOR_MAKE(115, 115, 115),
    .accent = LV_COLOR_MAKE(251, 191, 36),
    .accent2 = LV_COLOR_MAKE(180, 83, 9),
    .on = LV_COLOR_MAKE(251, 191, 36),
    .warm = LV_COLOR_MAKE(251, 191, 36),
    .heat = LV_COLOR_MAKE(245, 158, 11),
    .cool = LV_COLOR_MAKE(229, 229, 229),
    .violet = LV_COLOR_MAKE(212, 212, 212),
    .alert = LV_COLOR_MAKE(239, 68, 68),
    .ink_on = LV_COLOR_MAKE(0, 0, 0),
    .radius = 4,
    .radius_sm = 2,
    .name = "ink",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
