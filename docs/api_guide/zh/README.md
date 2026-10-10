# Customer SDK API 手册

> 本工程二次开发 API + **场景化开发指南**。  
> English: [../en/README.md](../en/README.md)

---

## 从这里开始（强烈推荐）

1. [**0. 二次开发怎么开始？**](./00_secondary_dev.md) — 心智模型、三种开发方式、学习路径  
2. [**AI 协作开发**](./AI_DEV.md) — 结合 Cursor/ChatGPT 的上下文与提示词（含红线）  
3. [**G00. 可做的产品方向**](./guides/G00_directions.md) — 7 种方向怎么选  
4. [**G01. Modbus + RS485 + LVGL**](./guides/G01_modbus_rs485_lvgl.md) — 最常见实战  

给 AI 置顶约束卡：[**AI_CONTEXT.md**](./AI_CONTEXT.md) · 提示词速查：[G06](./guides/G06_ai_prompts.md)

示例源码：`main/app/examples/app_bus_bridge_example.c`

---

## 场景指南（多技术融合）

| 指南 | 融合技术 |
|------|----------|
| [G00 方向总览](./guides/G00_directions.md) | 选型 |
| [G01 Modbus+RS485+LVGL](./guides/G01_modbus_rs485_lvgl.md) | 屏控现场设备 |
| [G02 Wi-Fi+MQTT+Hub](./guides/G02_wifi_mqtt_panel.md) | 云面板 |
| [G03 BLE/Mesh](./guides/G03_ble_local.md) | 近场控制 |
| [G04 以太网网关](./guides/G04_eth_rs485_gateway.md) | ETH+RS485 |
| [G05 全屋中控](./guides/G05_full_fusion.md) | 多入口融合 |
| [G06 AI 提示词](./guides/G06_ai_prompts.md) | 结合 AI 开发 |

### AI 协作

- [AI 协作二次开发指南](./AI_DEV.md)
- [AI_CONTEXT 约束卡](./AI_CONTEXT.md)（对话置顶）

---

## 技术分章

### 入门

- [概览与分层](./01_overview.md)
- [环境与编译](./02_build.md)
- [启动顺序](./03_boot.md)

### 网络与云

- [Wi-Fi](./04_wifi.md)
- [MQTT（巴法云）](./05_mqtt.md)
- [蓝牙 BLE / Mesh](./06_bt.md)
- [以太网](./07_eth.md)

### 外设与总线

- [背光](./08_backlight.md)
- [蜂鸣器](./09_beep.md)
- [GPIO_OUT / RS485（详细）](./10_gpio_rs485.md)
- [Modbus RTU（详细）](./11_modbus.md)
- [射频共存](./12_coexist.md)

### 系统

- [低功耗](./13_low_power.md)
- [跨线程 GUI（详细）](./14_gui_task.md)
- [健康监控](./17_health.md)

### 界面

- [Hub 模型与 UI（详细）](./15_hub.md)
- [图标更换](./16_icons.md)

### 参考

- [Kconfig](./18_kconfig.md)
- [控制通路](./19_control_map.md)
- [FAQ](./20_faq.md)
- [头文件索引](./A_headers.md)
