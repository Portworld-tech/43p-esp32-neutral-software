# 3. 启动顺序

```c
wifi_management_foundation_init();
app_low_power_init();
bt_management_init(...);          /* 可选 */
app_spiffs_mount();
board_display_start_with_lvgl_cfg(...);
board_ethernet_ch390_init/try_start();
board_backlight_init();
/* 可选: app_rs485_init(NULL)  — Kconfig APP_RS485_AUTO_INIT */
app_ui_start();
gui_task_init();
wifi_management_start();
bt_management_start();            /* 可选 */
```

显示初始化之后才有 I2C/扩展器，RS485 DE（GPIO_OUT）依赖此顺序。

---

[← 环境与编译](./02_build.md) | [目录](./README.md) | [Wi-Fi →](./04_wifi.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
