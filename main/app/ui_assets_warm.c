#include "sdkconfig.h"
#include "ui_assets_warm.h"

#if defined(CONFIG_APP_USE_SPIFFS_UI_ASSETS) && CONFIG_APP_USE_SPIFFS_UI_ASSETS && defined(CONFIG_LV_USE_PNG) && CONFIG_LV_USE_PNG

#include "esp_log.h"

static const char *TAG = "ui_assets";

void ui_assets_warm_spiffs_png_cache(void)
{
    /*
     * 勿在启动时把 SC1–SC4 全部 _lv_img_cache_open：每张都是全分辨率解码缓冲，
     * 与首页 ui_Image1 主题 + home 等叠加后极易占满 PSRAM，之后 Screen10 只能显示
     * 「当前」主题那一张（与 Image1 共用同一缓存路径），其余三张解码失败。
     * 主题图改为进入各屏时按需解码；若需预热单张小图可在此按需添加。
     */
    ESP_LOGD(TAG, "skip bulk theme PNG warm (SPIFFS full-res SC1–SC4 too heavy for PSRAM)");
}

#else /* !SPIFFS PNG */

void ui_assets_warm_spiffs_png_cache(void) {}

#endif
