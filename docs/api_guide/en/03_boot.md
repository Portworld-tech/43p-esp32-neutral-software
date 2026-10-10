# 3. Boot order

```c
wifi_management_foundation_init();
app_low_power_init();
bt_management_init(...);          /* optional */
app_spiffs_mount();
board_display_start_with_lvgl_cfg(...);
board_ethernet_ch390_init/try_start();
board_backlight_init();
/* optional: app_rs485_init(NULL)  — Kconfig APP_RS485_AUTO_INIT */
app_ui_start();
gui_task_init();
wifi_management_start();
bt_management_start();            /* optional */
```

I2C / expander become available only after display init. RS485 DE (`GPIO_OUT`) depends on that order.

---

[← Environment & build](./02_build.md) | [README](./README.md) | [Wi-Fi →](./04_wifi.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
