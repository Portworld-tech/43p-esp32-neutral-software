# Appendix A: Header index

## Open layer (prefer for secondary development)

| Feature | Header |
|---------|--------|
| Umbrella include | `main/app/app_api.h` |
| Beep pulse | `main/app/app_beep.h` |
| Coexistence | `main/app/app_coexist.h` |
| GPIO_OUT / DE | `main/app/app_gpio_out.h` |
| RS485 | `main/app/app_rs485.h` |
| Modbus | `main/app/app_modbus_rtu.h` |
| Wi-Fi helpers | `main/app/app_wifi_util.h` |
| Low power | `main/app/app_low_power.h` |
| GUI post | `main/gui/gui_task.h` |
| Expander (safe subset) | `components/board_bsp/include/board_io_expander.h` |

## Closed library public headers

| Feature | Header |
|---------|--------|
| Wi-Fi | `wifi_management.h` |
| MQTT | `wifi_bemfa_client.h` |
| Bluetooth | `bt_management.h` / `bt_proto.h` |
| Ethernet | `board_ethernet_ch390.h` |
| Backlight / beep HW | `withthewind_board_lvgl_init.h` |
| Hub model | `hub_model.h` |
| Hub UI | `hub_ui.h` |

Example (not in default build): `main/app/examples/app_bus_bridge_example.h`

---

[← FAQ](./20_faq.md) | [README](./README.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
