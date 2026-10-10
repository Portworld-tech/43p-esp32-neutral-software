# G02. Scenario: Wi-Fi + MQTT + Hub cloud panel

## User story

A phone (Bemfa cloud) and the touch panel share the same `hub_model` device state. After idle, the panel dims into ambient mode.

## Technologies

`wifi_management` · `wifi_bemfa_client` · `hub_model` / `hub_ui` · `UI_AMBIENT_*` · `board_backlight_*` · (optional) bus bridge → Modbus

## Steps

1. Enable `APP_FEATURE_MQTT=y` and join Wi-Fi from the network page  
2. Confirm MQTT connected in the log  
3. After local changes:

```c
hub_model_toggle_widget(room, slot);
hub_ui_refresh();
wifi_bemfa_client_schedule_sync();
```

4. If RS485 is also used: cloud and touch must share `app_bus_bridge_post_write` ([G01](./G01_modbus_rs485_lvgl.md))  
5. Ambient: `UI_AMBIENT_ENABLE` + `TIMEOUT_SEC` + `BACKLIGHT_PCT`

## Notes

- Default UID/topics live in the closed library; customize via vendor or your own MQTT client  
- Do not block on network I/O inside LVGL callbacks  

## See also

[Wi-Fi](../04_wifi.md) · [MQTT](../05_mqtt.md) · [Low power](../13_low_power.md)
