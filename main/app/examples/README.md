# Bus bridge example (Modbus + LVGL)

Not compiled by default. To use:

1. Copy `app_bus_bridge_example.c` → `../app_bus_bridge.c`
2. Copy `app_bus_bridge_example.h` → `../app_bus_bridge.h`
3. Add `app/app_bus_bridge.c` to `main/CMakeLists.txt` `MAIN_SRCS`
4. Call `app_bus_bridge_start()` after `gui_task_init()` in `main.c`
5. From theme callbacks: `app_bus_bridge_post_write(slave, addr, value)`

Guide: `docs/api_guide/zh/guides/G01_modbus_rs485_lvgl.md`
