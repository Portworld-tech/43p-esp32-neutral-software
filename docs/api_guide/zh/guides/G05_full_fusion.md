# G05. 场景：全屋中控多入口融合

## 用户故事

「回家」场景一键：屏显更新、云端同步、BLE 通知、多个 Modbus 从站批量动作。

## 融合技术

方式 1+2+3 全部：`hub_model_apply_scene` 为唯一场景入口。

## 推荐实现

```text
hub_model_apply_scene("home")
        │
        ├─► hub_ui_refresh()
        ├─► wifi_bemfa_client_schedule_sync()
        ├─► bt_management_mesh_send_set_state(...)  /* 可选 */
        └─► 队列批量 bus_job（灯、窗帘、空调设定点）
                 └─► app_modbus_write_single × N
```

## 设计原则

1. **单一真相：** 只信任 `hub_model`  
2. **单一桥接：** 所有写出总线走同一 `app_bus_bridge`  
3. **单一刷新：** 非 LVGL 线程用 `gui_task_post_lvgl`  
4. **失败可见：** toast + `protos[].health`  

## 相关

[G00 方向总览](./G00_directions.md) · [控制通路](../19_control_map.md) · [G01](./G01_modbus_rs485_lvgl.md)
