# G02. 场景：Wi-Fi + MQTT + Hub 云面板

## 用户故事

手机（巴法云）与触摸屏共同控制同一套 `hub_model` 设备态；无人操作后降亮待机。

## 融合技术

`wifi_management` · `wifi_bemfa_client` · `hub_model` / `hub_ui` · `UI_AMBIENT_*` · `board_backlight_*` ·（可选）`app_bus_bridge`→Modbus

## 步骤

1. `APP_FEATURE_MQTT=y`，屏上网络页连 Wi-Fi  
2. 确认日志 MQTT connected  
3. 本地改态后调用：

```c
hub_model_toggle_widget(room, slot);
hub_ui_refresh();
wifi_bemfa_client_schedule_sync();
```

4. 若同时有 RS485：云端命令路径与触摸共用 `bus_bridge_post_write`（见 [G01](./G01_modbus_rs485_lvgl.md)）  
5. 待机：`UI_AMBIENT_ENABLE` + `TIMEOUT_SEC` + `BACKLIGHT_PCT`

## 注意

- UID/主题在封闭库；定制联系厂商或自建 MQTT 客户端  
- 勿在 LVGL 回调里阻塞网络  

## 相关

[Wi-Fi](../04_wifi.md) · [MQTT](../05_mqtt.md) · [低功耗](../13_low_power.md)
