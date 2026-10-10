# AI 协作二次开发指南

> 用 Cursor / ChatGPT / Copilot 等 AI 辅助本工程开发时，按本文准备**上下文**和**提示词**，可显著减少乱改封闭库、在 LVGL 线程阻塞总线等常见错误。

---

## 1. 为什么要「带着文档问 AI」

本 SDK 分层特殊：

| 可改 | 不可改 / 勿臆造 |
|------|------------------|
| `main/`、`ui/themes/`、`spiffs_image/`、`main/app/app_*.c` | `components/*/lib/**/*.a` 内部实现 |
| 公开头文件里的 API | 测试台 `esp32_s3_frame` 的 `ui_testbench_*`（非产品 API） |
| 示例 `main/app/examples/` | 引脚表（未公开；用 `app_rs485_config_t` / 板级 API） |

AI 若只扫代码、不读文档，容易：

- 在按钮回调里直接 `app_modbus_write_single`（卡死触摸）
- 绕过 `hub_model` 只改 `lv_label`
- 去改/反编译 `.a` 或虚构不存在的 RS485 驱动头文件

**正确做法：** 先让 AI 读指定文档 + 头文件，再提需求。

---

## 2. 推荐工作流（客户侧）

```text
① 选方向（G00） → ② 打开对应指南（如 G01）
 → ③ 把「AI 上下文卡」+ 相关 md/头文件丢给 AI
 → ④ 用本文提示词模板提需求
 → ⑤ 人工检查：是否只改开放层、是否跨线程合规
 → ⑥ 板子上验证
```

| 步骤 | 你做什么 | AI 做什么 |
|------|----------|-----------|
| 定目标 | 选 G01～G05 之一 | — |
| 喂上下文 | @ 文档与头文件（见下节） | 理解约束 |
| 要方案 | 粘贴提示词模板 | 出文件级改动计划 |
| 要代码 | 确认计划后让它改 | 只改 `main/` / `ui/themes/` |
| 审查 | 对照「红线清单」 | 按你反馈修正 |

---

## 3. 应提供给 AI 的文件（按场景）

### 3.1 通用（每次建议带上）

| 文件 | 原因 |
|------|------|
| `docs/api_guide/zh/AI_CONTEXT.md` | 一页约束卡（专为 AI 设计） |
| `docs/api_guide/zh/00_secondary_dev.md` | 心智模型 |
| `main/app/app_api.h` | 开放 API 入口 |
| `components/hub_core/include/hub_model.h` | 状态机 API |

### 3.2 Modbus + RS485 + LVGL（最常见）

再附加：

- `docs/api_guide/zh/guides/G01_modbus_rs485_lvgl.md`
- `docs/api_guide/zh/10_gpio_rs485.md`
- `docs/api_guide/zh/11_modbus.md`
- `docs/api_guide/zh/14_gui_task.md`
- `main/app/app_rs485.h`、`app_modbus_rtu.h`
- `main/app/examples/app_bus_bridge_example.c`

### 3.3 云面板 / BLE / 网关

| 方向 | 附加文档 |
|------|----------|
| MQTT 云面板 | G02、`wifi_bemfa_client.h`、`wifi_management.h` |
| BLE | G03、`bt_management.h`、`app_coexist.h` |
| 以太网网关 | G04、`board_ethernet_ch390.h` |
| 全屋融合 | G05、G00 |

在 Cursor 中可用 `@文件` / `@文件夹` 引用；在 ChatGPT 等可粘贴 `AI_CONTEXT.md` + 相关章节全文。

---

## 4. 提示词模板（复制即用）

### 模板 A — 实现「屏控 Modbus 灯」

```text
你是 ESP32-S3 + LVGL Customer SDK 的二次开发助手。
必须遵守 docs/api_guide/zh/AI_CONTEXT.md 中的红线。

目标：触摸开关客厅灯，经 RS485 Modbus 写从站 1 寄存器 0（0/1）。

要求：
1. 参考 G01 与 main/app/examples/app_bus_bridge_example.c
2. LVGL/主题回调只改 hub_model + 入队；Modbus 在独立 FreeRTOS 任务
3. 只改 main/ 与 ui/themes/，禁止改 .a 与臆造引脚宏
4. 给出：需新建/修改的文件列表、CMake 改动、main.c 调用点、主题回调示例

先输出改动计划，等我确认后再写代码。
```

### 模板 B — 只改主题 UI

```text
基于 Customer SDK Hub 主题机制（见 15_hub.md）。
目标：在当前主题下把首页某按钮文案改为「一键离家」，点击 apply_scene("away") 并 hub_ui_refresh。
只改 ui/themes/<当前主题>/ 下文件，不要动总线与网络。
先说明要改哪个文件的哪个回调。
```

### 模板 C — 排查总线不通

```text
设备：Customer SDK，已 app_rs485_init(NULL)，modbus_read_holding 失败。
控制台可能仍占用 UART0。请按 10_gpio_rs485.md / 20_faq 给出排查清单（DE、A/B、波特率、控制台冲突、从站地址），
不要建议反编译 board_bsp.a。
```

### 模板 D — 融合 MQTT + Modbus

```text
在 G01 总线桥接已存在的前提下，让巴法 MQTT 下发的开关与触摸走同一 app_bus_bridge_post_write。
阅读 G02、G05、wifi_bemfa_client.h。说明应挂接的扩展点（apply_set_state / 开放层回调），
禁止在 LVGL 线程阻塞网络或 Modbus。
```

### 模板 E — 让 AI 先「读文档再回答」

```text
在回答前，请先列出你将依据的本仓库文档路径（必须包括 AI_CONTEXT.md）。
若文档与常识冲突，以本仓库文档为准。
不要使用 esp32_s3_frame 的 ui_testbench_* API。
```

---

## 5. 红线清单（给人或给 AI 审查用）

AI 或同事提交代码后，用此表快速验收：

| # | 检查项 | 通过标准 |
|---|--------|----------|
| 1 | 改动范围 | 仅 `main/`、`ui/themes/`、`spiffs_image/`、文档 |
| 2 | 状态源 | 设备态经 `hub_model_*`，刷新经 `hub_ui_refresh` / `gui_task_post_lvgl` |
| 3 | 总线线程 | 无在 LVGL 回调里 `read`/`transact`/`modbus_*` 长阻塞 |
| 4 | API 来源 | `#include` 来自 `app_*.h` / 公开 `components/*/include` |
| 5 | 封闭库 | 未修改 `.a`，未声明未公开符号 |
| 6 | UART | 已说明控制台与 RS485 的 UART 分配 |
| 7 | 示例 | 总线桥接参考 `examples/app_bus_bridge_example.c` |

---

## 6. 文档如何配合 AI（本手册结构说明）

| 文档类型 | AI 用法 |
|----------|---------|
| `AI_CONTEXT.md` | 系统提示 / 置顶约束 |
| `00_secondary_dev.md` | 解释架构 |
| `guides/G*.md` | 任务说明书（实现步骤） |
| `10/11/14/15_*.md` | API 细节与错误用法对照 |
| `A_headers.md` | 防止 AI 编造头文件路径 |
| `examples/*.c` | 少幻觉的代码起点 |

优化原则（维护文档时）：

1. **路径写死**：始终写仓库相对路径，便于 `@` 引用  
2. **正反例成对**：正确 / 错误代码各一段  
3. **场景可复制**：G01 级步骤可直接当 AI 任务描述  
4. **约束前置**：红线放在 AI_CONTEXT，避免埋在长文末尾  

---

## 7. 结合 Cursor 的实用技巧

1. 用 **Chat** 做方案，用 **Agent** 落文件；先计划后改代码  
2. `@docs/api_guide/zh/AI_CONTEXT.md` + `@guides/G01...` 固定开头  
3. 要求 AI：「列出将修改的文件，禁止修改 `components/*/lib`」  
4. 生成后让 AI 对照本章第 5 节红线自检一遍  
5. 板级问题（无应答）优先查文档 FAQ，再问 AI，避免它瞎改引脚  

---

## 8. 相关链接

| 文档 | 说明 |
|------|------|
| [AI_CONTEXT.md](./AI_CONTEXT.md) | 一页约束卡（优先喂给 AI） |
| [提示词速查](./guides/G06_ai_prompts.md) | 更多短提示词 |
| [二次开发入门](./00_secondary_dev.md) | 人读总览 |
| [G00 方向](./guides/G00_directions.md) | 选产品形态 |
| [G01 实战](./guides/G01_modbus_rs485_lvgl.md) | Modbus+LVGL |
