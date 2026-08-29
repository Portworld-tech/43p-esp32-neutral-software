/** Forest — palette */
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
    .bg_deep = LV_COLOR_MAKE(12, 20, 16),
    .bg_base = LV_COLOR_MAKE(18, 28, 22),
    .bg_card = LV_COLOR_MAKE(24, 36, 28),
    .bg_card2 = LV_COLOR_MAKE(33, 48, 40),
    .bg_elev = LV_COLOR_MAKE(42, 59, 50),
    .line = LV_COLOR_MAKE(40, 55, 45),
    .t1 = LV_COLOR_MAKE(232, 245, 236),
    .t2 = LV_COLOR_MAKE(181, 207, 192),
    .t3 = LV_COLOR_MAKE(122, 154, 136),
    .t4 = LV_COLOR_MAKE(85, 112, 96),
    .accent = LV_COLOR_MAKE(110, 231, 183),
    .accent2 = LV_COLOR_MAKE(5, 150, 105),
    .on = LV_COLOR_MAKE(110, 231, 183),
    .warm = LV_COLOR_MAKE(252, 211, 77),
    .heat = LV_COLOR_MAKE(251, 191, 36),
    .cool = LV_COLOR_MAKE(94, 234, 212),
    .violet = LV_COLOR_MAKE(167, 243, 208),
    .alert = LV_COLOR_MAKE(251, 113, 133),
    .ink_on = LV_COLOR_MAKE(5, 46, 26),
    .radius = 14,
    .radius_sm = 9,
    .name = "forest",
};

const hub_palette_t *hub_palette(void)
{
    return &s_pal;
}
