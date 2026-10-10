# 20. FAQ

| Problem | What to do |
|---------|------------|
| Blank icons | Flash **storage** (SPIFFS); confirm PNG paths + POSIX FS |
| RS485 no reply | Check DE polarity; console vs UART0; `app_rs485_is_ready()` |
| Missing expander symbols at link | Customer subset restored in `board_io_expander.h`; ensure `board_bsp.a` is linked |
| No beep on press | Use `app_beep_click_if_enabled` or check click-sound in `hub_model` |
| Touch freezes when controlling devices | Move Modbus/RS485 off the LVGL thread — see [G01](./guides/G01_modbus_rs485_lvgl.md) |
| ETH drops during Modbus | Ensure writes go through `app_rs485_write` (coexist pause) |
| AI suggests editing `.a` / ui_testbench | Reject — pin [AI_CONTEXT](./AI_CONTEXT.md) |

---

[← Control map](./19_control_map.md) | [README](./README.md) | [Header index →](./A_headers.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
