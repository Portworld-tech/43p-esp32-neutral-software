# SM43P / 4 寸 ESP32 面板 — 客户常见问题（FAQ）

面向采购、硬件集成与二次开发工程师。  
产品形态：**ESP32-S3 + 4 寸 480×480 触摸屏（SM43P 类）+ 中性 Hub SDK**。  

- 中文原稿：[CUSTOMER_FAQ_CN.md](./CUSTOMER_FAQ_CN.md)  
- **English:** [CUSTOMER_FAQ_EN.md](./CUSTOMER_FAQ_EN.md)  

更细的二次开发见 [THEME_SECONDARY_DEV.md](./THEME_SECONDARY_DEV.md) 与 [CUSTOMER_API_GUIDE.md](./CUSTOMER_API_GUIDE.md)。

---

## 0. 一分钟结论

| 主题 | 结论 |
|------|------|
| 开发环境 | **推荐 VS Code / Cursor + ESP-IDF 5.5.x**；Arduino 仅适合小 Demo |
| 交付形态 | 中性分层 SDK：封闭板级/云/模型库 + 开放主题与业务胶水 |
| 温湿度 | 可选 **AHT20**，与触摸共用 **I2C（IO7=SCL，IO15=SDA）**；插上模块 + 打开 `AHT20_ENABLE` 即可自动读并显示；示值偏差用 `main/board/aht20_calib.h` 宏校准（见 §1.13） |
| 蜂鸣器 / RTC | 电路支持；蜂鸣器可做按键音；RTC 电池焊盘支持，出厂可不装电池 |
| RS-485 | 标准半双工；默认 **8N1** / Modbus；**MARK/SPACE 粘滞校验非标配**（见 §1.14） |
| GUI | **LVGL + Hub 主题 + ESP-IDF**；可用 SquareLine / GUI Guider 等设计（见 §1.15） |
| Home Assistant | 无官方现成 HA 集成包；协议层可自行对接（MQTT 等），厂商可做定制 |
| 图标 | 无内置图标编辑器；用 iconfont / 设计工具导出 PNG 替换即可 |

---

## 1. 已答复问题（归纳）

### 1.1 内置温湿度模块后，还要重新调软件吗？

**一般不需要。**  
中性固件已接好 AHT20 采集与界面显示路径。正确安装模块并启用 `CONFIG_AHT20_ENABLE` 后，后台会轮询温湿度，写入 `hub_model`，各 Hub 主题自动刷新显示。

> 若客户自研固件、未使用本 SDK，则需自行对接驱动。

---

### 1.2 支持 Arduino 开发吗？

| 场景 | 建议 |
|------|------|
| 小功能验证（扫 Wi‑Fi、拉一下蜂鸣器） | Arduino / PlatformIO 可以做**小体量 Demo** |
| 智能家居整机 / 本面板量产 UI | **不推荐 Arduino**；请用 **VS Code + ESP-IDF** |

原因：本产品依赖 ESP-IDF 组件、LVGL 刷屏、PSRAM、双核任务与分层 `.a` 库，Arduino 抽象层难以稳定承载完整工程。

---

### 1.3 板上有 RTC 电池焊盘吗？

**是。** 板级预留 RTC 电池焊盘，**硬件支持**外接电池维持时钟域。  
当前出货常见为**未焊接 RTC 电池**；需要掉电走时再装配即可（具体电池规格以硬件说明书为准）。

---

### 1.4 能否加装小型蜂鸣器做触摸声反馈？

**可以。** 开发板电路支持数字输出驱动**压电有源/无源小型蜂鸣器**（非功放扬声器方案）。  
软件侧使用公开 API（如 `board_beep_set` / `app_beep_*`，以交付头文件为准），设置页也可开关按键音。

---

### 1.5 顶部连接器 / I2C / 温湿度 / 模拟口

| 问题 | 说明 |
|------|------|
| 顶部连接器用途 | 当前交付语境下，**I2C 用于温湿度等外设**（与触摸总线策略见板级说明） |
| 是否 I2C | **是 I2C** |
| 能否接温湿度 | **可以**；推荐 **AHT20**（与触摸共用总线） |
| 温湿度引脚 | **SCL = IO7，SDA = IO15** |
| 模拟温度探头 | 本中性 SDK **未把 ADC 温度探头作为标准交付路径**；若需 NTC 等，需评估空闲 ADC 脚与硬件改版，属定制范围 |

---

### 1.6 能否对接 Home Assistant（HA）？

| 项 | 说明 |
|------|------|
| 是否有官方 HA 插件 | **目前没有**单独适配的「开箱即用 HA 集成包」 |
| 能否兼容 | **可以兼容**：设备侧可通过 **MQTT / HTTP** 等与 HA 互通 |
| 厂商能力 | 中性软件未强制绑定某一云；曾做过类似云控 / 小程序方案，**可按项目定制 HA 对接** |

客户短期可走：设备连 Wi‑Fi → MQTT Broker → HA MQTT 集成；中长期可让厂商做实体/发现模板。

---

### 1.7 温湿度接在哪？引脚多少？

- 模块：**AHT20**（可选外插 / 板载方案以订单硬件为准）  
- 总线：与触摸共用 **I2C**  
- 引脚：**IO7（SCL）、IO15（SDA）**  
- 地址：常见 `0x38`（以 `menuconfig` / `AHT20_I2C_ADDR` 为准）

---

### 1.8 如何用 Arduino / PlatformIO 读温湿度？有说明吗？

**正式交付不走 Arduino / PlatformIO 主路径。**  

请使用：

1. **VS Code + ESP-IDF**  
2. 本仓库 **中性 Customer SDK**  
3. `menuconfig` 打开 `AHT20_ENABLE`  
4. 烧录含 SPIFFS 的完整固件  

插上传感器后，后台任务会自动采集；也可在开放层自行调用：

```c
#include "aht20.h"
#include "withthewind_board_lvgl_init.h"

aht20_init(board_i2c_get_handle());

float t_c = 0.0f, rh = 0.0f;
aht20_read(&t_c, &rh);              /* 校准后：温度 °C + 湿度 %RH */
aht20_read_raw(&t_c, &rh);          /* 未校准原始值（调 OFFSET 时对照用） */
/* 或 */
aht20_read_temperature_c(&t_c);
aht20_read_humidity_rh(&rh);

/* 界面同源数据（已走校准后的 aht20_read） */
hub_model()->indoor_c;   /* °C */
hub_model()->rh;         /* %RH 整数 */
```

现场示值偏差请改 `main/board/aht20_calib.h` 宏后重新编译烧录（见 **§1.13**）。  
无传感器时，界面可能显示演示默认值，属预期行为。

---

### 1.9 是否提供 ESP-IDF 驱动与源码 / 其他开发选项？

| 提供 | 不提供（或受限） |
|------|------------------|
| 中性 Customer SDK（ESP-IDF 工程） | 完整私有产品仓 |
| 开放：`ui/themes`、`hub_ui`、部分 `main/app`、AHT20 源码等 | 完整 GPIO/扩展器引脚表源码 |
| 公开头文件 + 预编译 `board_bsp` / `hub_core` / `cloud_wifi` / `bt_ctrl` | 封闭库 `.c` 反编译/再分发 |
| 十套 Hub 主题二次开发、文档与示例 | SquareLine `default` 主题包（客户包不含） |

客户可在中性软件上改 UI/UX、接 RS485/Modbus/MQTT、扩展业务逻辑。

---

### 1.10 Arduino 不够用，还有别的方案吗？

有，且这是**推荐方案**：

- **IDE**：VS Code / Cursor  
- **框架**：**ESP-IDF 5.5.x**  
- **工程**：本目录 Customer SDK（`idf.py set-target esp32s3` → `build` → `flash`）  
- **UI 预览（浏览器）**：仓库内 / GitHub Pages 的 `lvgl-front` 主题选型包  

Arduino 仅建议用于极小 Demo，不适合本面板完整智能家居项目。

---

### 1.11 有没有做图标的系统或工具？

**没有内置图标编辑器 GUI。**  

推荐流程：

1. 在 [iconfont 阿里巴巴矢量图标库](https://www.iconfont.cn/) 或 Figma / Illustrator 等导出图标  
2. 做成 **白图 + 透明底、约 96×96 PNG**  
3. 覆盖 `spiffs_image/icons/nt/<name>.png`（名称与 `hub_icons.c` 一致）  
4. `idf.py build flash`（必须刷入 **storage/SPIFFS**）

可选：厂商脚本可由 `lvgl-front` 矢量定义批量生成同风格遮罩 PNG。

---

### 1.12 如何管理显示屏（背光 / 主题 / 待机）？

不是单独的「显示操作系统」，而是 **板级 API + Hub UI**：

| 需求 | 做法 |
|------|------|
| 开关背光 / 调亮度 | `board_backlight_on/off`、`board_backlight_set(0..100)` |
| 设置页亮度 | 已接 `hub_device_*` / 设置页滑条 |
| 换主题 / 改页面 | 改 `main/app_ui_theme_select.h`；页面在 `ui/themes/<主题>/` 与 `main/hub_ui/` |
| 待机降亮 | `UI_AMBIENT_*` + `app_low_power`（见文档） |

显示内容由 **LVGL + `hub_model`** 驱动；客户改主题或写模型即可，无需改封闭 BSP 引脚源码。

---

### 1.13 如何读取温度和湿度？如何用宏校准到实际值？

**自动路径（推荐）**

1. 硬件：AHT20 接到 I2C（IO7/IO15）  
2. 软件：`CONFIG_AHT20_ENABLE=y`  
3. API：`aht20_read`（带校准）/ `aht20_read_raw`（不带校准）；界面数据见 `hub_model()->indoor_c` / `rh`  
4. UI：各 Hub 主题首页 / 能耗等相关页已绑定模型字段，有传感器后会刷新为实测值  

**客户宏校准（开放层，改完需 rebuild + flash）**

编辑文件：**`main/board/aht20_calib.h`**。

公式：

```text
温度_out(°C) = 温度_raw × AHT20_TEMP_SCALE + AHT20_TEMP_OFFSET_C
湿度_out(%)  = 湿度_raw × AHT20_RH_SCALE   + AHT20_RH_OFFSET_PCT
```

| 宏 | 默认 | 作用 |
|----|------|------|
| `AHT20_CALIB_ENABLE` | `1` | `0` = 关闭全部校准，只出传感器换算值 |
| `AHT20_TEMP_OFFSET_C` | `0.0f` | 温度偏移（°C），偏高则填负数 |
| `AHT20_TEMP_SCALE` | `1.0f` | 温度增益 |
| `AHT20_RH_OFFSET_PCT` | `0.0f` | 湿度偏移（%RH） |
| `AHT20_RH_SCALE` | `1.0f` | 湿度增益 |
| `AHT20_RH_CLAMP` | `1` | 校准后湿度夹紧到 0..100 |

示例（屏上比标准表高 1.5°C、湿度低 3%）：

```c
/* main/board/aht20_calib.h */
#define AHT20_TEMP_OFFSET_C   (-1.5f)
#define AHT20_RH_OFFSET_PCT   (3.0f)
```

调参建议：先用 `aht20_read_raw` 对照参考表 → 改 OFFSET/SCALE → `idf.py build flash` → 确认界面示值。  
校准在驱动层生效，Hub UI / 后台轮询 / 客户调用 `aht20_read*` 一致；勿在主题里再减一遍偏移。

---

### 1.14 RS-485 是否支持 MARK 和 SPACE 模式？

客户说的 MARK/SPACE 常有两种含义，答复前先对齐：

| 含义 | SM43P / 中性 SDK | 口径 |
|------|------------------|------|
| **电气层** 空闲 Mark / 有效 Space（差分 A/B） | 标准 RS-485 半双工收发 + DE/RE | **支持**（物理层固有行为） |
| **帧格式** Stick parity：地址字节 Mark、数据字节 Space（第 9 位多机寻址） | 中性层默认 **8N1**；ESP-IDF UART 公开配置为无/偶/奇；`app_rs485` **未暴露** MARK/SPACE | **非标配、非开箱即用**；强制需要时评估定制 |

工程现状（开放层）：

- `app_rs485_init` 默认：`UART_DATA_8_BITS` + `UART_PARITY_DISABLE` + `UART_STOP_BITS_1`，波特率可配（常见 115200 / 9600）
- 常规 **Modbus RTU**、自定义 8 位帧：走现有 `app_rs485_*` / `app_modbus_*` 即可
- 详见 `main/app/app_rs485.h` 与 [THEME_SECONDARY_DEV.md](./THEME_SECONDARY_DEV.md) 第 12 节

**建议对外答复（中文）：**

> SM43P 的 RS-485 为标准半双工电气接口（UART + DE/RE），物理层符合 RS-485 差分电平（空闲为 Mark 态）。  
> 软件侧中性 SDK 默认按 **8N1** 半双工通信，并提供 Modbus RTU 等常用用法；波特率等可在开放层配置。  
> 若贵司指的是串口 **MARK/SPACE 粘滞校验（地址/数据第 9 位区分）**：当前交付固件与客户 API **未将该模式作为标准功能开放**。ESP32-S3 的 ESP-IDF UART 配置以无/偶/奇校验为主。若现场协议强制要求 MARK/SPACE，请提供协议说明，我们可评估定制实现；常规 Modbus/自定义 8 位帧不依赖该模式即可对接。

**Customer-facing reply (English):**

> The RS-485 port on the SM43P is a standard half-duplex electrical interface (UART + DE/RE). At the physical layer it follows RS-485 differential signaling (idle bus is in the Mark state).  
> On the software side, our neutral SDK defaults to **8N1** half-duplex communication and supports common usages such as Modbus RTU. Baud rate and related parameters can be configured in the open application layer.  
> If you mean UART **MARK/SPACE stick parity** (using the 9th bit to distinguish address vs. data bytes): that mode is **not exposed as a standard feature** in the delivered firmware or customer APIs. The ESP32-S3 ESP-IDF UART configuration primarily supports no / even / odd parity. If your field protocol strictly requires MARK/SPACE, please share the protocol specification and we can evaluate a custom implementation. Typical Modbus or custom 8-bit frames do not depend on that mode and can be integrated as-is.

> **对内注意：** 勿笼统说「硬件完全支持 MARK/SPACE 串口模式」；须区分物理层 Mark/Space 与 stick parity，并索要对方协议文档。

---

### 1.15 GUI 如何实现？如何创建应用界面？用了哪些工具/框架/库？

| 层级 | 技术 |
|------|------|
| 图形库 | **LVGL**（设备端渲染） |
| 系统 / 构建 | **ESP-IDF 5.5.x**（推荐 VS Code / Cursor） |
| 显示 / 触摸 | 板级 BSP 封装（如 ST7701 RGB **480×480**、GT911）；应用不直接改封闭引脚表 |
| 业务数据 | `hub_model`（温度、房间、场景、协议状态等） |
| UI 壳层 / 路由 | `main/hub_ui/` |
| 外观与布局 | 十套开放 **Hub 主题** `ui/themes/<主题>/` |
| 资源 | SPIFFS 图标等（须 flash **storage**） |
| 选型预览（可选） | 浏览器 **lvgl-front** / GitHub Pages；量产以固件内 LVGL 主题为准 |

**如何创建 / 修改应用界面：**

1. 开发环境：VS Code / Cursor + ESP-IDF（完整项目不推荐 Arduino）  
2. 在 `app_ui_theme_select.h` 选择主题，或基于 `_template` 新建主题  
3. 修改主题内页面（如 `home.c`、`pages_*.c`）与配色（`palette.c`），用 LVGL API 搭控件  
4. 图标等资源放 SPIFFS（如 PNG），烧录时需写入 storage 分区  
5. 可选：浏览器 `lvgl-front` 做主题选型预览；量产 UI 以固件内 LVGL 主题源码为准  

**工具 / 框架 / 库：** ESP-IDF、LVGL、`esp_lvgl_port`、板级 LCD/触摸组件；业务侧为中性 Hub SDK。客户包不含 SquareLine 的 default 工程；界面主要以 **SquareLine / GUI Guider** 等独立桌面 GUI 编辑器进行设计。

细文档：[THEME_SECONDARY_DEV.md](./THEME_SECONDARY_DEV.md)

**建议对外答复（中文）：**

> SM43P 图形界面在设备上由 **LVGL** 实现，运行于 **ESP-IDF**（推荐 5.5.x），显示驱动（如 ST7701 RGB 480×480）与触摸（如 GT911）由板级 BSP 封装，应用不直接改底层引脚。  
>  
> **界面架构：** 业务数据 `hub_model`；UI 壳层与路由 `hub_ui`；外观与布局为十套开放 Hub 主题（`ui/themes/<主题名>/`）。  
> **如何创建/修改：** VS Code + ESP-IDF；选/建主题；改页面与配色；资源进 SPIFFS 并刷 storage；可选 `lvgl-front` 预览。  
> **工具 / 框架 / 库：** ESP-IDF、LVGL、`esp_lvgl_port`、板级 LCD/触摸组件、中性 Hub SDK。客户包不含 SquareLine default；界面主要以 SquareLine / GUI Guider 等独立桌面 GUI 编辑器进行设计。

**Customer-facing reply (English):**

> The SM43P on-device GUI is implemented with **LVGL**, running on **ESP-IDF** (recommended **5.5.x**). The display driver (e.g. ST7701 RGB **480×480**) and touch controller (e.g. GT911) are wrapped by the board-level BSP, so applications do not modify low-level pin maps directly.  
>  
> **UI architecture:** `hub_model` (data), `hub_ui` (shell/routing), ten open Hub themes under `ui/themes/<theme_name>/`.  
> **How to create / modify:** VS Code / Cursor + ESP-IDF; select or create a theme; edit pages/colors with the LVGL API; put assets in SPIFFS and flash **storage**; optional `lvgl-front` preview.  
> **Tools / frameworks / libraries:** ESP-IDF, LVGL, `esp_lvgl_port`, board LCD/touch components, neutral Hub SDK. The customer package does **not** include a SquareLine default project; UI work is primarily done with standalone desktop GUI editors such as **SquareLine** or **GUI Guider**.

---

## 2. 客户可能继续问的问题（预测 + 建议答复）

### 2.1 硬件与接口

| # | 可能问题 | 建议答复要点 |
|---|----------|--------------|
| P1 | Flash / PSRAM 多大？ | 以规格书为准（常见 **16MB Flash + 8MB PSRAM**）；OTA 双分区见 `partitions.csv` |
| P2 | 有没有以太网？ | 可选 CH390 SPI 以太网（板级封装）；与 Wi‑Fi/BLE 共存时注意内存 |
| P3 | USB 是下载口还是 Host？ | **为下载口** |
| P4 | 能否外接摄像头 / 喇叭？ | 非标准交付；需硬件评估与定制软件 |
| P5 | RS485 引脚在哪？有没有隔离？ | SDK 开放层有 UART/RS485 示例框架；**具体丝印/隔离以硬件手册为准**，勿假设客户包含完整 pinmux 源码 |
| P5b | RS485 是否 MARK/SPACE？ | 见 **§1.14**：电气层 Mark/Space 有；粘滞校验非标配；默认 8N1 / Modbus |
| P6 | 接近传感器接口怎么用？ | 若硬件预留 I2C 扩展，优先走共享 I2C；驱动需客户或定制开发 |
| P7 | 工作电压 / 功耗 / 认证？ | 工作电压为 **3V3**；低功耗见 `app_low_power` / `UI_AMBIENT_*` |

### 2.2 软件与交付

| # | 可能问题 | 建议答复要点 |
|---|----------|--------------|
| P8 | 能否给全部源码？ | 提供**中性分层 SDK**；板级/云/模型核心以 `.a` + 头文件交付，主题与业务开放 |
| P9 | 和 GitHub 上公开仓库什么关系？ | 公开仓为可分享的中性示例/预览；量产以合同交付的 SDK 包为准 |
| P10 | 十套主题能否商用？ | 中性 Hub 主题供 OEM 二次开发；第三方客户专有 UI 资源勿混入 |
| P11 | 如何 OTA？ | 分区支持双 OTA；具体升级通道（HTTP/MQTT）可基于 IDF 扩展或定制 |
| P12 | 巴法云 / 自有 MQTT？ | 封闭库含巴法路径；自有 Broker 可在开放层另建客户端，注意与默认 MQTT 开关隔离 |
| P13 | 多语言？ | Hub 模型支持中/英切换（`hub_model` 设置） |
| P14 | 没有屏能不能当网关跑？ | 理论上可裁剪 UI，但当前交付以带屏中控为主；无头定制评估 |

### 2.3 温湿度与传感器

| # | 可能问题 | 建议答复要点 |
|---|----------|--------------|
| P15 | 精度 / 标定？ | 遵循 AHT20 规格；现场先改 **`main/board/aht20_calib.h`** 宏（§1.13）；需要掉电保存时可再扩展 NVS |
| P16 | 能否换 SHT30 / DHT22？ | 非标；需改驱动与总线占用评估（DHT 非 I2C） |
| P17 | 两个温度：室内与设定点？ | `indoor_c`/`rh` 为环境传感；空调设定点在房间 widget / `hub_model` 设备字段 |
| P18 | 插上不显示？ | 查：模块焊接/方向、`AHT20_ENABLE`、I2C 冲突、日志 `AHT20 ready`、是否刷了新固件 |

### 2.4 显示与 UI

| # | 可能问题 | 建议答复要点 |
|---|----------|--------------|
| P19 | 分辨率？ | **480×480** |
| P20 | 能用 SquareLine 直接出货？ | 客户包**不含** SquareLine default；用十套 Hub 或自建 Hub 风格页；可用 SquareLine / GUI Guider 设计后落到主题 |
| P20b | GUI 用什么做的？怎么改界面？ | 见 **§1.15**：LVGL + Hub 主题 + ESP-IDF |
| P21 | 浏览器预览？ | `lvgl-front`：`npx serve .` 或 GitHub Pages 在线预览 |
| P22 | 图标空白？ | 必须 flash **storage**；检查 PNG + `LV_USE_PNG` + FS POSIX |
| P23 | 亮度档位为何跳跃？ | 旧板扩展器软件 PWM 有档位；新板 GPIO LEDC 可更平滑（以交付板型为准） |

### 2.5 云与生态

| # | 可能问题 | 建议答复要点 |
|---|----------|--------------|
| P24 | 微信小程序 / App？ | 无强制捆绑；可按项目定制（曾有类似小程序能力） |
| P25 | 免端口映射远程运维？ | 需设备**出站**连云（MQTT/WSS）+ 云门户；局域网 Web 可另做 |
| P26 | Matter / HomeKit？ | 非当前中性 SDK 标配；需专项评估 |
| P27 | 数据安全 / 账号体系？ | 中性包侧重设备侧；云账号与审计属云侧定制 |

### 2.6 商务与支持

| # | 可能问题 | 建议答复要点 |
|---|----------|--------------|
| P28 | 最小起订 / 交期 / 定制周期？ | 商务口径（本文不写死数字） |
| P29 | 培训与技术支持？ | 提供文档 + SDK；深度联调可签支持/定制合同 |
| P30 | 授权与再分发？ | 封闭 `.a` 勿逆向；开放层按合同授权给终端客户二次开发 |

---

## 3. 销售 / 支持话术对照（短答）

| 客户原话 | 一句话 |
|----------|--------|
| 插上温湿度还要改软件吗？ | 不用，中性固件已接好，启用后自动显示。 |
| 温湿度和标准表对不上？ | 改 `main/board/aht20_calib.h` 的 OFFSET/SCALE 宏后重新编译烧录。 |
| 能不能 Arduino 做全屋？ | 不适合；用 VS Code + ESP-IDF + 中性 SDK。 |
| 有没有 RTC 电池？ | 有焊盘支持，默认可不装电池。 |
| 要蜂鸣提示音？ | 可以，板子支持小型数字蜂鸣器。 |
| 顶接口干什么？ | I2C，温湿度走 IO7/IO15。 |
| 能不能上 HA？ | 无官方插件，但可用 MQTT 等兼容，可定制。 |
| 图标怎么做？ | 用 iconfont 等导出 PNG 替换 SPIFFS。 |
| 屏怎么管？ | 背光 API + 主题宏 + hub_model，不是独立显示 OS。 |
| RS485 支持 MARK/SPACE 吗？ | 电气层有；粘滞校验非标配。默认 8N1，Modbus 可用；要 stick parity 需定制评估。 |
| GUI 怎么做的？ | LVGL + ESP-IDF + Hub 主题；可用 SquareLine / GUI Guider 设计。 |
| 源码都给吗？ | 给中性分层 SDK：主题开放，板级核心库封闭交付。 |

---

## 4. 相关文档索引

| 文档 | 内容 |
|------|------|
| [THEME_SECONDARY_DEV.md](./THEME_SECONDARY_DEV.md) | 工程总览、主题、图标、Wi‑Fi/MQTT、AHT20、RS485 |
| GitHub 公开示例 | [Portworld-tech/43p-esp32-neutral-software](https://github.com/Portworld-tech/43p-esp32-neutral-software) |
| 主题在线预览 | [GitHub Pages](https://portworld-tech.github.io/43p-esp32-neutral-software/) |

---

## 5. 修订记录

| 版本 | 日期 | 说明 |
|------|------|------|
| 1.0 | 2026-10-07 | 首版：汇总客户问答 + 预测问题 |
| 1.1 | 2026-10-08 | 增补 §1.14 RS-485 MARK/SPACE、§1.15 GUI |
| 1.2 | 2026-10-08 | §1.14 / §1.15 中英文对外答复 |
| 1.3 | 2026-10-08 | AHT20 宏校准 `aht20_calib.h` + `aht20_read_raw` |
| 1.4 | 2026-10-08 | 合并人工修订口径（8MB PSRAM、USB 下载口、3V3 等）并保留 485/GUI/温湿度校准新内容 |
