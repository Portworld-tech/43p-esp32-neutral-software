/** Pulse — palette */
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
    .bg_deep = LV_COLOR_MAKE(3, 6, 15),
    .bg_base = LV_COLOR_MAKE(7, 11, 24),
    .bg_card = LV_COLOR_MAKE(12, 24, 48),
    .bg_card2 = LV_COLOR_MAKE(16, 32, 64),
    .bg_elev = LV_COLOR_MAKE(24, 48, 88),
    .line = LV_COLOR_MAKE(20, 50, 70),
    .t1 = LV_COLOR_MAKE(232, 247, 255),
    .t2 = LV_COLOR_MAKE(158, 200, 224),
    .t3 = LV_COLOR_MAKE(90, 138, 170),
    .t4 = LV_COLOR_MAKE(58, 96, 120),
    .accent = LV_COLOR_MAKE(0, 240, 255),
    .accent2 = LV_COLOR_MAKE(0, 144, 160),
    .on = LV_COLOR_MAKE(57, 255, 20),
    .warm = LV_COLOR_MAKE(255, 204, 0),
    .heat = LV_COLOR_MAKE(255, 107, 0),
    .cool = LV_COLOR_MAKE(0, 240, 255),
    .violet = LV_COLOR_MAKE(192, 132, 252),
    .alert = LV_COLOR_MAKE(255, 45, 85),
    .ink_on = LV_COLOR_MAKE(2, 16, 24),
    .radius = 2,
    .radius_sm = 2,
    .name = "pulse",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
