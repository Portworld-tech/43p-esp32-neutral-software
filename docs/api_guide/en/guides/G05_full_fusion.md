# G05. Scenario: whole-home multi-channel hub

## User story

One “Home” scene: UI updates, cloud sync, BLE notify, and batch Modbus actions to several slaves.

## Technologies

Directions 1+2+3: `hub_model_apply_scene` is the only scene entry point.

## Recommended flow

```text
hub_model_apply_scene("home")
        │
        ├─► hub_ui_refresh()
        ├─► wifi_bemfa_client_schedule_sync()
        ├─► bt_management_mesh_send_set_state(...)  /* optional */
        └─► queued batch of bus_jobs
                 └─► app_modbus_write_single × N
```

## Design rules

1. **Single truth:** trust only `hub_model`  
2. **Single bridge:** all bus writes go through one `app_bus_bridge`  
3. **Single refresh path:** non-LVGL threads use `gui_task_post_lvgl`  
4. **Visible failures:** toast + `protos[].health`  

## See also

[G00 directions](./G00_directions.md) · [Control map](../19_control_map.md) · [G01](./G01_modbus_rs485_lvgl.md)
