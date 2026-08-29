/** Bloom — palette */
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
    .bg_deep = LV_COLOR_MAKE(246, 240, 242),
    .bg_base = LV_COLOR_MAKE(239, 230, 234),
    .bg_card = LV_COLOR_MAKE(248, 242, 245),
    .bg_card2 = LV_COLOR_MAKE(255, 248, 250),
    .bg_elev = LV_COLOR_MAKE(232, 220, 226),
    .line = LV_COLOR_MAKE(210, 195, 200),
    .t1 = LV_COLOR_MAKE(58, 42, 50),
    .t2 = LV_COLOR_MAKE(110, 86, 98),
    .t3 = LV_COLOR_MAKE(154, 132, 144),
    .t4 = LV_COLOR_MAKE(184, 168, 176),
    .accent = LV_COLOR_MAKE(232, 145, 168),
    .accent2 = LV_COLOR_MAKE(196, 92, 120),
    .on = LV_COLOR_MAKE(90, 171, 122),
    .warm = LV_COLOR_MAKE(232, 160, 106),
    .heat = LV_COLOR_MAKE(217, 119, 6),
    .cool = LV_COLOR_MAKE(126, 184, 201),
    .violet = LV_COLOR_MAKE(184, 160, 208),
    .alert = LV_COLOR_MAKE(214, 69, 93),
    .ink_on = LV_COLOR_MAKE(255, 248, 250),
    .radius = 28,
    .radius_sm = 16,
    .name = "bloom",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
