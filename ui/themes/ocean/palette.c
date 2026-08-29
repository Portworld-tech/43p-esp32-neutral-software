/** Ocean — palette */
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
    .bg_deep = LV_COLOR_MAKE(7, 16, 24),
    .bg_base = LV_COLOR_MAKE(12, 24, 36),
    .bg_card = LV_COLOR_MAKE(18, 36, 52),
    .bg_card2 = LV_COLOR_MAKE(26, 49, 68),
    .bg_elev = LV_COLOR_MAKE(35, 64, 86),
    .line = LV_COLOR_MAKE(30, 50, 70),
    .t1 = LV_COLOR_MAKE(238, 246, 255),
    .t2 = LV_COLOR_MAKE(182, 205, 224),
    .t3 = LV_COLOR_MAKE(122, 152, 176),
    .t4 = LV_COLOR_MAKE(77, 106, 128),
    .accent = LV_COLOR_MAKE(56, 189, 248),
    .accent2 = LV_COLOR_MAKE(2, 132, 199),
    .on = LV_COLOR_MAKE(74, 222, 128),
    .warm = LV_COLOR_MAKE(251, 191, 36),
    .heat = LV_COLOR_MAKE(251, 146, 60),
    .cool = LV_COLOR_MAKE(34, 211, 238),
    .violet = LV_COLOR_MAKE(129, 140, 248),
    .alert = LV_COLOR_MAKE(248, 113, 113),
    .ink_on = LV_COLOR_MAKE(8, 47, 73),
    .radius = 10,
    .radius_sm = 6,
    .name = "ocean",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
