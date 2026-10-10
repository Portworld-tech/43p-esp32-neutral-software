# 18. Kconfig

| Option | Meaning |
|--------|---------|
| `APP_FEATURE_MQTT` / `BLE` / `MESH` | Cloud / Bluetooth features |
| `APP_ENABLE_RS485` | Compile RS485 / Modbus APIs |
| `APP_RS485_AUTO_INIT` | Call `app_rs485_init` at boot (default **n**) |
| `UI_AMBIENT_*` | Idle backlight / ambient mode |
| `APP_HEALTH_MONITOR` | Heap / health monitor |

Configure with `idf.py menuconfig` under the Customer SDK / secondary-dev menus. Keep `APP_RS485_AUTO_INIT` off until UART console conflict is resolved ([RS485](./10_gpio_rs485.md)).

---

[← Health monitor](./17_health.md) | [README](./README.md) | [Control map →](./19_control_map.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
