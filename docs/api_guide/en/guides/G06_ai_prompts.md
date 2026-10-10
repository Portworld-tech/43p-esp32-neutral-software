# G06. AI prompt cheat sheet

Use with [AI-assisted development](../AI_DEV.md) and [AI_CONTEXT.md](../AI_CONTEXT.md). Replace `…` with your concrete goal.

---

## 1. Session opener (recommended every time)

```text
Please read and obey @docs/api_guide/en/AI_CONTEXT.md .
This repo is Customer SDK — edit open layers only.
Do not use ui_testbench APIs; do not modify .a files.
```

---

## 2. Short prompts by direction

**Modbus light control**

```text
Follow @docs/api_guide/en/guides/G01_modbus_rs485_lvgl.md ,
integrate examples/app_bus_bridge_example into the project,
theme callback: living-room light toggle → post_write(1,0,on).
List files first.
```

**Theme reskin only**

```text
Edit only ui/themes and spiffs_image. See @docs/api_guide/en/15_hub.md
and @docs/api_guide/en/16_icons.md . Goal: …
```

**MQTT + same bus**

```text
bus_bridge already exists. Read G02/G05 so cloud on/off shares post_write with touch.
```

**BLE point**

```text
Override bt_management_apply_set_state, update hub_model, optional post_write.
Wrap scans with app_coexist_before/after_ble_scan. See G03.
```

**Build / link errors**

```text
Analyze the log; prefer A_headers and CMakeLists;
do not suggest editing lib/*.a contents.
```

---

## 3. Ask the AI to review

```text
Review my latest diff against AI_DEV.md section 5 red lines.
List violations and fixes only (do not rewrite unrelated files).
```

---

## 4. Commit message help

```text
From the current diff, write a 1–2 sentence English commit message
focused on why (e.g. add Modbus bridge so the panel can drive lights).
```
