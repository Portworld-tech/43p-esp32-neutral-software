# G00. 客户可做的二次开发方向（多技术融合）

> 本章列举 **可落地的产品形态**，每个方向都会融合本工程已有技术。选一个方向后，按对应专题指南实现。

---

## 总览表

| 方向 | 融合技术 | 难度 | 专题 |
|------|----------|------|------|
| ① 屏端 Modbus 控制器 | LVGL + Hub + RS485 + Modbus + 蜂鸣 | ★★☆ | [G01](./G01_modbus_rs485_lvgl.md) |
| ② 云端智能面板 | LVGL + Wi-Fi + MQTT + Hub + 背光待机 | ★★☆ | [G02](./G02_wifi_mqtt_panel.md) |
| ③ 近场 BLE 控制 | LVGL + BLE/Mesh + Hub + 共存 | ★★★ | [G03](./G03_ble_local.md) |
| ④ 有线网关 | 以太网 + RS485/Modbus + Hub 运维页 | ★★★ | [G04](./G04_eth_rs485_gateway.md) |
| ⑤ 全屋场景中控 | ①+②+③ 多入口统一 hub_model | ★★★★ | [G05](./G05_full_fusion.md) |
| ⑥ 低功耗展示终端 | 待机层 + 背光 + SNTP/Wi-Fi + 只读传感器 | ★★☆ | 见下文「方式 6」 |

---

## 方式 1 — 屏端 Modbus 控制器（最贴合「RS485+LVGL」）

**用户故事：** 触摸屏上点「客厅灯」，经 RS485 Modbus 写从站线圈/寄存器，灯亮，屏上状态同步。

```text
按钮 → hub_model_toggle → 队列 → 总线任务
         → app_modbus_write_single → 从站
         → 成功则 hub_ui_refresh + app_beep_pulse
```

**你会用到：** `hub_ui` / `hub_model`、`gui_task`、`app_rs485`、`app_modbus_rtu`、`app_beep`、`app_coexist`（写总线时自动 pause ETH）。

→ 详细步骤：[G01](./G01_modbus_rs485_lvgl.md)

---

## 方式 2 — 云端智能面板

**用户故事：** 手机巴法 App 开关设备，屏上实时变；本地触摸也能改，并回传云端。

```text
MQTT 命令 →（库内）→ hub_model
触摸 → hub_model → wifi_bemfa_client_schedule_sync()
```

**你会用到：** `wifi_management`、`wifi_bemfa_client`、`hub_model`、`UI_AMBIENT_*` 待机。

可与方式 1 叠加：云端命令最终也走同一 `app_bus_bridge` 写 Modbus。

→ [G02](./G02_wifi_mqtt_panel.md)

---

## 方式 3 — 近场 BLE / Mesh 控制

**用户故事：** 微信小程序或配网器近场控灯；屏上也显示。

```text
BLE SET_STATE → bt_management_apply_set_state（你覆盖）
              → hub_model + 可选 Modbus 写出
```

扫描前调用 `app_coexist_before_ble_scan()`，避免与以太网 SPI 抢资源。

→ [G03](./G03_ble_local.md)

---

## 方式 4 — 以太网 + RS485 网关

**用户故事：** 网线进中控，对上 TCP/HTTP，对下 RS485 从站；运维页显示链路健康。

```text
ETH got IP → 自研 TCP 服务 → 解析 → app_modbus_* 
hub_model()->protos[] 更新 Wi-Fi/ETH/RS485 健康度
```

→ [G04](./G04_eth_rs485_gateway.md)

---

## 方式 5 — 全屋场景中控（多入口融合）

**用户故事：** 「回家」场景：屏上一点，同时 MQTT 上报、BLE 通知邻居节点、Modbus 批量写多个从站。

统一规则：

1. 场景只改 `hub_model_apply_scene`  
2. 桥接任务订阅 model 变更（或显式 enqueue 一批写命令）  
3. 所有入口禁止直接改 LVGL，只改 model  

→ [G05](./G05_full_fusion.md)

---

## 方式 6 — 低功耗信息屏 / 只读仪表

**用户故事：** 走廊屏显示温湿度与时间，无人操作后降亮待机；偶发 Wi-Fi 同步。

**技术：** `app_low_power_init`、`UI_AMBIENT_*`、`board_backlight_set`、AHT20（`ui_bg_task`）、可选以太网只收不发。

可不启用 RS485；与方式 1 正交。

---

## 方式 7 — 主题 OEM 换皮

只改 `ui/themes/` + `spiffs_image/icons`，业务仍走 `hub_model`。适合品牌商快速出货。见 [图标更换](../16_icons.md)、[Hub UI](../15_hub.md)。

---

## 怎么选？

| 你的目标 | 选 |
|----------|----|
| 「不会云，先把 RS485 灯控起来」 | 方式 1 |
| 「要对接手机 App」 | 方式 2（可再加 1） |
| 「工地近场调试」 | 方式 3 |
| 「机房有网线」 | 方式 4 |
| 「要做完整中控产品」 | 方式 5 |
| 「只要展示，少交互」 | 方式 6 |
| 「换 UI 皮肤」 | 方式 7 |

---

## 架构对照（所有方向共用）

```text
开放层可改：
  ui/themes/*          外观
  main/hub_ui/*        壳层（谨慎）
  main/app/app_*.c     总线 / 蜂鸣 / 共存
  main/main.c          启动钩子

封闭层只调用头文件：
  hub_model / wifi_* / bt_* / board_*
```

下一篇推荐：若你要控实体设备，直接打开 [G01 Modbus+RS485+LVGL](./G01_modbus_rs485_lvgl.md)。  
若使用 **Cursor / ChatGPT 辅助写代码**，先打开 [AI 协作指南](../AI_DEV.md)，对话置顶 [AI_CONTEXT.md](../AI_CONTEXT.md)。
