# 附录 A：头文件索引

## 开放层（二次开发优先）

| 功能 | 头文件 |
|------|--------|
| 总入口 | `main/app/app_api.h` |
| 蜂鸣脉冲 | `main/app/app_beep.h` |
| 共存 | `main/app/app_coexist.h` |
| GPIO_OUT/DE | `main/app/app_gpio_out.h` |
| RS485 | `main/app/app_rs485.h` |
| Modbus | `main/app/app_modbus_rtu.h` |
| Wi-Fi 工具 | `main/app/app_wifi_util.h` |
| 低功耗 | `main/app/app_low_power.h` |
| GUI 投递 | `main/gui/gui_task.h` |
| 扩展器（安全子集） | `components/board_bsp/include/board_io_expander.h` |

## 封闭库公开头

| 功能 | 头文件 |
|------|--------|
| Wi-Fi | `wifi_management.h` |
| MQTT | `wifi_bemfa_client.h` |
| 蓝牙 | `bt_management.h` / `bt_proto.h` |
| 以太网 | `board_ethernet_ch390.h` |
| 背光/蜂鸣 | `withthewind_board_lvgl_init.h` |
| Hub 模型 | `hub_model.h` |

---

[← FAQ](./20_faq.md) | [目录](./README.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
