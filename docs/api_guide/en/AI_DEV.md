# AI-Assisted Secondary Development

> When using Cursor, ChatGPT, Copilot, or similar tools on this SDK, prepare **context** and **prompts** as described here. That sharply reduces mistakes such as editing closed `.a` libraries or blocking Modbus inside LVGL callbacks.

---

## 1. Why feed the docs to the AI

This SDK is intentionally layered:

| You may change | Do not change / invent |
|----------------|------------------------|
| `main/`, `ui/themes/`, `spiffs_image/`, `main/app/app_*.c` | Internals of `components/*/lib/**/*.a` |
| Public header APIs | Testbench `esp32_s3_frame` `ui_testbench_*` (not product APIs) |
| Examples under `main/app/examples/` | Full pin maps (unpublished; use `app_rs485_config_t` / board APIs) |

If the model only skims source without docs, it often:

- Calls `app_modbus_write_single` directly from a button callback (freezes touch)
- Bypasses `hub_model` and only updates `lv_label`
- Tries to edit or reverse `.a` files, or invents non-existent RS485 headers

**Correct approach:** make the AI read the listed docs and headers first, then state the task.

---

## 2. Recommended customer workflow

```text
① Pick a direction (G00) → ② Open the matching guide (e.g. G01)
 → ③ Attach AI_CONTEXT + related markdown/headers
 → ④ Use a prompt template from this page
 → ⑤ Human review: open layer only? thread-safe?
 → ⑥ Validate on hardware
```

| Step | You | AI |
|------|-----|-----|
| Goal | Choose G01–G05 | — |
| Context | `@` docs and headers | Learn constraints |
| Plan | Paste a template | File-level change plan |
| Code | Confirm the plan | Edit only `main/` / `ui/themes/` |
| Review | Red-line checklist | Fix per your feedback |

---

## 3. Files to attach (by scenario)

### 3.1 Always useful

| File | Why |
|------|-----|
| `docs/api_guide/en/AI_CONTEXT.md` | One-page constraint card for AI |
| `docs/api_guide/en/00_secondary_dev.md` | Mental model |
| `main/app/app_api.h` | Open API umbrella |
| `components/hub_core/include/hub_model.h` | Device-state API |

### 3.2 Modbus + RS485 + LVGL (most common)

Also attach:

- `docs/api_guide/en/guides/G01_modbus_rs485_lvgl.md`
- `docs/api_guide/en/10_gpio_rs485.md`
- `docs/api_guide/en/11_modbus.md`
- `docs/api_guide/en/14_gui_task.md`
- `main/app/app_rs485.h`, `app_modbus_rtu.h`
- `main/app/examples/app_bus_bridge_example.c`

### 3.3 Cloud / BLE / gateway

| Direction | Also attach |
|-----------|-------------|
| MQTT cloud panel | G02, `wifi_bemfa_client.h`, `wifi_management.h` |
| BLE | G03, `bt_management.h`, `app_coexist.h` |
| Ethernet gateway | G04, `board_ethernet_ch390.h` |
| Whole-home fusion | G05, G00 |

In Cursor use `@file` / `@folder`. In ChatGPT paste `AI_CONTEXT.md` plus the relevant chapters.

---

## 4. Prompt templates (copy-paste)

### Template A — On-panel Modbus light control

```text
You are a secondary-development assistant for the ESP32-S3 + LVGL Customer SDK.
Obey the red lines in docs/api_guide/en/AI_CONTEXT.md.

Goal: a touch toggle for the living-room light writes Modbus slave 1, register 0 (0/1) over RS485.

Requirements:
1. Follow G01 and main/app/examples/app_bus_bridge_example.c
2. LVGL/theme callbacks only update hub_model + enqueue; Modbus runs in a FreeRTOS worker
3. Edit only main/ and ui/themes/; do not modify .a files or invent pin macros
4. Deliver: file list, CMake changes, main.c hook, theme callback sample

Output a change plan first; wait for my confirmation before writing code.
```

### Template B — Theme UI only

```text
Use the Customer SDK Hub theme model (see 15_hub.md).
Goal: change a home-page button label to "Leave home"; on click call apply_scene("away") and hub_ui_refresh.
Edit only files under ui/themes/<current theme>/. Do not touch bus or networking.
First name which file and callback you will change.
```

### Template C — Bus not responding

```text
Customer SDK: app_rs485_init(NULL) done; modbus_read_holding fails.
Console may still own UART0. Using 10_gpio_rs485.md / 20_faq, give a checklist
(DE, A/B, baud, console conflict, slave address).
Do not suggest reverse-engineering board_bsp.a.
```

### Template D — MQTT + same bus bridge

```text
Assume the G01 bus bridge already exists. Make Bemfa MQTT on/off commands
share app_bus_bridge_post_write with touch. Read G02, G05, wifi_bemfa_client.h.
Name the extension points. Never block network or Modbus on the LVGL thread.
```

### Template E — Force docs-first answers

```text
Before answering, list the repo doc paths you will rely on (must include AI_CONTEXT.md).
If docs conflict with generic knowledge, prefer this repo.
Do not use esp32_s3_frame ui_testbench_* APIs.
```

---

## 5. Red-line checklist (human or AI review)

| # | Check | Pass criteria |
|---|--------|----------------|
| 1 | Scope | Only `main/`, `ui/themes/`, `spiffs_image/`, docs |
| 2 | State | Device state via `hub_model_*`; refresh via `hub_ui_refresh` / `gui_task_post_lvgl` |
| 3 | Bus threads | No long `read`/`transact`/`modbus_*` inside LVGL callbacks |
| 4 | APIs | Includes from `app_*.h` / public `components/*/include` |
| 5 | Closed libs | No `.a` edits; no undeclared private symbols |
| 6 | UART | Console vs RS485 UART assignment documented |
| 7 | Example | Bus bridge based on `examples/app_bus_bridge_example.c` |

---

## 6. How the manual is structured for AI

| Doc type | AI use |
|----------|--------|
| `AI_CONTEXT.md` | System / pinned constraints |
| `00_secondary_dev.md` | Architecture |
| `guides/G*.md` | Task playbooks |
| `10/11/14/15_*.md` | API detail and anti-patterns |
| `A_headers.md` | Prevent invented include paths |
| `examples/*.c` | Low-hallucination code start |

Maintenance principles:

1. **Hard-code paths** for `@` mentions  
2. **Pair good/bad examples**  
3. **Scenario steps** usable as AI task text (G01-level)  
4. **Constraints first** in AI_CONTEXT, not buried at the end  

---

## 7. Cursor tips

1. Use **Chat** for plans, **Agent** for file edits; plan before large diffs  
2. Start with `@docs/api_guide/en/AI_CONTEXT.md` + `@guides/G01...`  
3. Require: “list files to change; never modify `components/*/lib`”  
4. After generation, ask the model to self-check against section 5  
5. For “no slave response”, check FAQ first so the model does not invent pins  

---

## 8. Related links

| Doc | Role |
|-----|------|
| [AI_CONTEXT.md](./AI_CONTEXT.md) | Constraint card (pin first) |
| [G06 prompts](./guides/G06_ai_prompts.md) | Short prompt cheat sheet |
| [Secondary development](./00_secondary_dev.md) | Human overview |
| [G00 directions](./guides/G00_directions.md) | Product shapes |
| [G01 walkthrough](./guides/G01_modbus_rs485_lvgl.md) | Modbus + LVGL |
| Chinese edition | [../zh/AI_DEV.md](../zh/AI_DEV.md) |
