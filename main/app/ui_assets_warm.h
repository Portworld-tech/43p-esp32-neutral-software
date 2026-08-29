#pragma once

/**
 * Optional hook after ui_init(); SPIFFS 全分辨率主题 PNG 不再在此批量预热（避免占满 PSRAM）。
 */
void ui_assets_warm_spiffs_png_cache(void);
