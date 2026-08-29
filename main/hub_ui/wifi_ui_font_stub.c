#include "wifi_ui_font.h"
#include "hub_font.h"

void wifi_ui_font_init(void)
{
    hub_font_init();
}

const lv_font_t *wifi_ui_font_ssid(void)
{
    return hub_font();
}
