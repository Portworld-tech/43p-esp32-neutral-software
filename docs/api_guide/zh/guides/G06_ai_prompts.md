# G06. AI 提示词速查

配合 [AI 协作指南](../AI_DEV.md) 与 [AI_CONTEXT.md](../AI_CONTEXT.md) 使用。复制后把 `…` 换成你的具体需求。

---

## 1. 开场（每次建议先发）

```text
请阅读并遵守 @docs/api_guide/zh/AI_CONTEXT.md 。
本仓库是 Customer SDK，只改开放层。不要用 ui_testbench API，不要改 .a。
```

---

## 2. 按方向短提示

**Modbus 控灯**

```text
按 @docs/api_guide/zh/guides/G01_modbus_rs485_lvgl.md ，
把 examples/app_bus_bridge_example 接入工程，
主题回调：客厅灯 toggle → post_write(1,0,on)。
先给文件清单。
```

**换主题皮肤**

```text
只改 ui/themes 与 spiffs_image，参考 @docs/api_guide/zh/15_hub.md @docs/api_guide/zh/16_icons.md 。
目标：…
```

**MQTT + 同一总线**

```text
已有 bus_bridge。阅读 G02/G05，让云端开关与触摸共用 post_write。
```

**BLE 点位**

```text
覆盖 bt_management_apply_set_state，更新 hub_model，可选 post_write。
扫描用 app_coexist_before/after_ble_scan。见 G03。
```

**编译/链接报错**

```text
根据报错日志分析；优先查 A_headers 与 CMakeLists；
禁止建议修改 lib/*.a 内容。
```

---

## 3. 让 AI 做审查

```text
请审查我刚改的 diff，对照 AI_DEV.md 第 5 节红线清单，
列出违规项与修改建议（不要直接大改无关文件）。
```

---

## 4. 让 AI 写提交说明（若你用 git）

```text
根据当前改动写 1～2 句中文 commit message，聚焦「为何」：
例如接入 Modbus 桥接以便屏控灯。
```
