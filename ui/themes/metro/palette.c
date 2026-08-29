/** Metro — palette */
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
    .bg_deep = LV_COLOR_MAKE(27, 27, 27),
    .bg_base = LV_COLOR_MAKE(34, 34, 34),
    .bg_card = LV_COLOR_MAKE(45, 45, 45),
    .bg_card2 = LV_COLOR_MAKE(56, 56, 56),
    .bg_elev = LV_COLOR_MAKE(68, 68, 68),
    .line = LV_COLOR_MAKE(55, 55, 55),
    .t1 = LV_COLOR_MAKE(255, 255, 255),
    .t2 = LV_COLOR_MAKE(208, 208, 208),
    .t3 = LV_COLOR_MAKE(154, 154, 154),
    .t4 = LV_COLOR_MAKE(106, 106, 106),
    .accent = LV_COLOR_MAKE(0, 188, 242),
    .accent2 = LV_COLOR_MAKE(0, 120, 212),
    .on = LV_COLOR_MAKE(16, 124, 16),
    .warm = LV_COLOR_MAKE(255, 140, 0),
    .heat = LV_COLOR_MAKE(232, 17, 35),
    .cool = LV_COLOR_MAKE(0, 188, 242),
    .violet = LV_COLOR_MAKE(135, 100, 184),
    .alert = LV_COLOR_MAKE(232, 17, 35),
    .ink_on = LV_COLOR_MAKE(255, 255, 255),
    .radius = 0,
    .radius_sm = 0,
    .name = "metro",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
