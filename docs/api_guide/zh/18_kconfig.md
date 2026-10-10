# 18. Kconfig

| 项 | 说明 |
|----|------|
| `APP_FEATURE_MQTT/BLE/MESH` | 云 / 蓝牙 |
| `APP_ENABLE_RS485` | 编译 RS485/Modbus API |
| `APP_RS485_AUTO_INIT` | 启动时 `app_rs485_init`（默认 n） |
| `UI_AMBIENT_*` | 待机背光 |
| `APP_HEALTH_MONITOR` | 堆监控 |

---

[← 健康监控](./17_health.md) | [目录](./README.md) | [控制通路 →](./19_control_map.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
