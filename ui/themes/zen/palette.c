/** Zen — palette */
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
    .bg_deep = LV_COLOR_MAKE(247, 247, 245),
    .bg_base = LV_COLOR_MAKE(240, 240, 237),
    .bg_card = LV_COLOR_MAKE(255, 255, 255),
    .bg_card2 = LV_COLOR_MAKE(245, 245, 242),
    .bg_elev = LV_COLOR_MAKE(232, 232, 228),
    .line = LV_COLOR_MAKE(229, 229, 227), /* FE rgba(20,20,18,0.08) on paper */
    .t1 = LV_COLOR_MAKE(26, 26, 24),
    .t2 = LV_COLOR_MAKE(92, 92, 88),
    .t3 = LV_COLOR_MAKE(142, 142, 136),
    .t4 = LV_COLOR_MAKE(181, 181, 176),
    .accent = LV_COLOR_MAKE(26, 26, 24),
    .accent2 = LV_COLOR_MAKE(0, 0, 0),
    .on = LV_COLOR_MAKE(47, 111, 78),
    .warm = LV_COLOR_MAKE(138, 122, 96),
    .heat = LV_COLOR_MAKE(180, 83, 9),
    .cool = LV_COLOR_MAKE(61, 107, 122),
    .violet = LV_COLOR_MAKE(107, 98, 128),
    .alert = LV_COLOR_MAKE(180, 35, 24),
    .ink_on = LV_COLOR_MAKE(247, 247, 245),
    .radius = 0,
    .radius_sm = 0,
    .name = "zen",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
