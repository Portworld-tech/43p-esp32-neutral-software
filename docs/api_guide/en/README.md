# Customer SDK API Guide (English)

> Secondary-development APIs and **scenario guides** for this product SDK.  
> 中文：[../zh/README.md](../zh/README.md)

---

## Start here (recommended)

1. [**0. Getting started with secondary development**](./00_secondary_dev.md) — mental model, three approaches, learning path  
2. [**AI-assisted development**](./AI_DEV.md) — Cursor/ChatGPT context and prompts (incl. red lines)  
3. [**G00. Product directions**](./guides/G00_directions.md) — how to pick among 7 shapes  
4. [**G01. Modbus + RS485 + LVGL**](./guides/G01_modbus_rs485_lvgl.md) — most common hands-on path  

Pin for AI: [**AI_CONTEXT.md**](./AI_CONTEXT.md) · Prompt cheat sheet: [G06](./guides/G06_ai_prompts.md)

Example source: `main/app/examples/app_bus_bridge_example.c`

---

## Scenario guides (multi-tech fusion)

| Guide | Focus |
|-------|--------|
| [G00 Directions overview](./guides/G00_directions.md) | Choose a product shape |
| [G01 Modbus+RS485+LVGL](./guides/G01_modbus_rs485_lvgl.md) | Drive field devices from the panel |
| [G02 Wi-Fi+MQTT+Hub](./guides/G02_wifi_mqtt_panel.md) | Cloud panel |
| [G03 BLE/Mesh](./guides/G03_ble_local.md) | Near-field control |
| [G04 Ethernet gateway](./guides/G04_eth_rs485_gateway.md) | ETH + RS485 |
| [G05 Whole-home hub](./guides/G05_full_fusion.md) | Multi-entry fusion |
| [G06 AI prompts](./guides/G06_ai_prompts.md) | Working with AI |

### AI collaboration

- [AI-assisted secondary development](./AI_DEV.md)
- [AI_CONTEXT constraint card](./AI_CONTEXT.md) (pin at top of the chat)

---

## Technical chapters

### Getting started

- [Overview & layers](./01_overview.md)
- [Environment & build](./02_build.md)
- [Boot order](./03_boot.md)

### Network & cloud

- [Wi-Fi](./04_wifi.md)
- [MQTT (Bemfa)](./05_mqtt.md)
- [Bluetooth BLE / Mesh](./06_bt.md)
- [Ethernet](./07_eth.md)

### Peripherals & bus

- [Backlight](./08_backlight.md)
- [Buzzer](./09_beep.md)
- [GPIO_OUT / RS485 (detailed)](./10_gpio_rs485.md)
- [Modbus RTU (detailed)](./11_modbus.md)
- [RF coexistence](./12_coexist.md)

### System

- [Low power](./13_low_power.md)
- [Cross-thread GUI (detailed)](./14_gui_task.md)
- [Health monitor](./17_health.md)

### UI

- [Hub model & UI (detailed)](./15_hub.md)
- [Icons](./16_icons.md)

### Reference

- [Kconfig](./18_kconfig.md)
- [Control map](./19_control_map.md)
- [FAQ](./20_faq.md)
- [Header index](./A_headers.md)
