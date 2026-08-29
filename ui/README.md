# UI theme packs (LVGL 8) 鈥?Customer SDK

Compile-time hub themes live under `themes/`. SquareLine `default` is **not** shipped.

| Pack | Path |
|------|------|
| slate 鈥?metro | `themes/<id>/` |

**閫夊瀷锛?* 缂栬緫 `main/app_ui_theme_select.h` 鐨?`APP_UI_THEME_ID`锛堝 `APP_UI_THEME_SLATE`锛夛紝鍐?`idf.py reconfigure && idf.py build`銆?

Board pins are embedded in closed `board_bsp.a` (not published as source).
