# 19. 控制通路

所有通道应汇聚到 **`hub_model`**，再决定是否写出总线 / 上云。

| 入口 | 建议数据流 |
|------|-----------|
| 触摸 UI | `hub_model_*` →（可选）`app_bus_bridge_post_write` → Modbus |
| 巴法 MQTT | 库内点位 → 同桥接写出 → `schedule_sync` |
| BLE SET_STATE | `apply_set_state` → model → 同桥接 |
| 以太网 TCP | 自研解析 → `app_modbus_*` → model 刷新 |
| 场景一键 | `apply_scene` → 批量队列写 |

**开发方式选型** → [G00](./guides/G00_directions.md)  
**屏控 RS485 实战** → [G01](./guides/G01_modbus_rs485_lvgl.md)
