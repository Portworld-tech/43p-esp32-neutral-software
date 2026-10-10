# 15. Hub model & UI (detailed)

> `hub_model` is the **single source of truth** for on-screen device state. Themes only decide “how it looks”; on/off semantics always go through model APIs.

---

## 1. Core headers

| Header | Use |
|--------|-----|
| `hub_model.h` | Rooms / widgets / scenes / settings / toast |
| `hub_ui.h` | `hub_ui_init/go/refresh`, routing |
| `hub_theme.h` | Theme hooks |
| `hub_device_ui.h` | Network page, brightness |
| `hub_icons.h` | SPIFFS icons |

---

## 2. Common model APIs

```c
hub_model_t *m = hub_model();

hub_model_set_room(0);
hub_model_apply_scene("home");
hub_model_toggle_widget(room, slot);
hub_model_step_widget(room, slot, +5);
hub_model_set_widget_level(room, slot, 80);
hub_model_toast("Done");
hub_ui_refresh();   /* refresh current page after model changes */
```

| Type `hub_wtype_t` | Typical use |
|--------------------|-------------|
| `HUB_W_ONOFF` | Light / relay |
| `HUB_W_DIMMER` | Dimmer |
| `HUB_W_CURTAIN` / `SHUTTER` | Curtain / shutter |
| `HUB_W_CLIM` / `THERMO` | AC / floor heat |
| `HUB_W_PLUG` / `FAN` | Plug / fan |

---

## 3. Theme secondary development

1. Switch theme: `main/app_ui_theme_select.h` → `APP_UI_THEME_ID`  
2. Colors: `ui/themes/<id>/palette.c`  
3. Layout: `home.c` / `pages_*.c`  
4. Must implement: `hub_theme_build(parent, route)`  

Button callback template (when wired to the bus):

```c
hub_model_toggle_widget(room, slot);
hub_ui_refresh();
app_beep_click_if_enabled();
app_bus_bridge_post_write(slave, addr, on ? 1 : 0);  /* see examples */
```

---

## 4. Fusion with other channels

| Channel | How to update UI |
|---------|------------------|
| Touch | Direct `hub_model_*` + `hub_ui_refresh` |
| Modbus poll task | Mutate model + `gui_task_post_lvgl(refresh)` |
| MQTT / BLE | Mutate model + refresh + (optional) bus write-out |

Related: [Getting started](./00_secondary_dev.md) · [G01](./guides/G01_modbus_rs485_lvgl.md) · [G00](./guides/G00_directions.md)
