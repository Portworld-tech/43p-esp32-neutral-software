# Customer SDK 二次开发总览（从零到量产定制）

本文面向拿到 `customer_sdk/` 的客户工程师：从工程是什么、能改什么、怎么编译烧录，到主题/图标、Hub 业务模型、**AHT20 温湿度示值调整**、**RS485 / Modbus 控设备**，以及 Wi‑Fi / 巴法 MQTT / 蓝牙扩展。

请把本目录当作**独立 ESP-IDF 工程**使用，不要与厂商私有产品仓根目录混开。

| 文档 | 用途 |
|------|------|
| **本文** | 二次开发总览（建议从这里开始） |
| [api_guide/zh/00_secondary_dev.md](./api_guide/zh/00_secondary_dev.md) | 心智模型 + 三种开发方式 + 学习路径 |
| [api_guide/zh/guides/G01_modbus_rs485_lvgl.md](./api_guide/zh/guides/G01_modbus_rs485_lvgl.md) | **屏控 RS485/Modbus 实战**（最常跟） |
| [api_guide/zh/guides/G00_directions.md](./api_guide/zh/guides/G00_directions.md) | 7 种产品融合方向选型 |
| [CUSTOMER_FAQ_CN.md](./CUSTOMER_FAQ_CN.md) | 售前/硬件集成 FAQ（AHT20/蜂鸣等） |
| [api_guide/zh/AI_DEV.md](./api_guide/zh/AI_DEV.md) | 结合 Cursor / ChatGPT 协作（含红线） |

English API guide: [api_guide/en/README.md](./api_guide/en/README.md)

---

## 0. 五分钟结论

| 问题 | 答案 |
|------|------|
| 这是什么？ | ESP32-S3 + LVGL 智能中控固件的**分层客户包**：板级/云/蓝牙/业务模型以预编译库交付；UI 主题、资源、开放外设 API 可改 |
| 目标芯片 | `esp32s3`（预编译 `.a` 仅此目标） |
| IDF 版本 | **ESP-IDF 5.5.x** |
| 默认 UI | 十套 Hub 主题之一（默认 `SLATE`），**不含** SquareLine `default` |
| 温湿度怎么显示？ | AHT20 → `ui_bg_task` → `hub_model()->indoor_c` / `rh` → 主题首页（如 Slate 温度/湿度卡片） |
| 示值怎么校准？ | 改开放层 **`main/board/aht20_calib.h`** 宏后重新编译烧录（见 §11） |
| 怎么控 RS485 设备？ | 开放层 **`app_rs485_*` / `app_modbus_*`** + 总线桥接任务（见 §12）；**禁止**在 LVGL 回调里阻塞读写 |
| 烧录注意 | 必须刷入 **storage（SPIFFS）**，否则图标空白 |

统一开放外设入口：

```c
#include "app_api.h"   /* RS485 / Modbus / GPIO_OUT / 蜂鸣 / 共存 / Wi-Fi 工具 */
```

---

## 1. 产品与软件架构

### 1.1 硬件能力（板级已封装）

典型交付板（Withthewind 类）：

- MCU：ESP32-S3
- 显示：ST7701 RGB **480×480**
- 触摸：GT911（I2C：SCL=IO7，SDA=IO15）
- IO 扩展：NCA9555（蜂鸣、RS485 DE 等）
- 背光：MCU **GPIO4 LEDC** 软件 PWM（`board_bsp.a`）
- 可选：AHT20 温湿度（与触摸共用 I2C）、CH390 SPI 以太网
- RS485：开放层默认 **UART0 · TX43 · RX44 · 115200** + 扩展器 DE（可配置）

**完整引脚表与扩展器网络不在客户包内公开**；请通过公开头文件操作：

- `components/board_bsp/include/withthewind_board_lvgl_init.h`
- `components/board_bsp/include/board_io_expander.h`（安全子集）
- `components/board_bsp/include/board_ethernet_ch390.h`
- `components/board_bsp/include/aht20.h`
- `main/app/app_*.h`（RS485 / Modbus / 蜂鸣 / 共存）

### 1.2 分层模型（开放 / 封闭）

```
┌──────────────────────────────────────────────────────────────┐
│  开放层（源码可改）                                             │
│  ui/themes/* · main/hub_ui · main/app · main/board           │
│  main/gui · spiffs_image · main/main.c                       │
└──────────────────────────▲───────────────────────────────────┘
                           │ 调用公开 API
┌──────────────────────────┴───────────────────────────────────┐
│  封闭层（仅 .a + include/*.h）                                  │
│  board_bsp · hub_core · cloud_wifi · bt_ctrl                   │
└──────────────────────────────────────────────────────────────┘
                           │
                    ESP-IDF / LVGL / 驱动组件
```

| 开放（可改） | 封闭（`.a`，勿改、勿反汇编） |
|---|---|
| `ui/themes/<主题>/` | `components/board_bsp`（屏/触摸/背光/ETH 封装） |
| `main/hub_ui/`、`main/gui/` | `components/hub_core`（`hub_model` 实现） |
| `main/app/`（RS485/Modbus/蜂鸣/共存…） | `components/cloud_wifi`（Wi‑Fi + 巴法 MQTT） |
| `main/board/aht20*.c`、`aht20_calib.h` | `components/bt_ctrl`（BLE / Mesh） |
| `main/app_ui_theme_select.h`、`main/main.c` | |
| `spiffs_image/`、`main/app/examples/` | |

封闭库路径：`components/*/lib/esp32s3/*.a`。

### 1.3 运行时数据流（控制从哪进、到哪出）

```
手机 App / 巴法云 ──MQTT──► cloud_wifi ──┐
手机 BLE / Mesh   ────────► bt_ctrl     ──┤
触摸屏 UI         ────────► hub_model_* ◄─┘──► hub_ui_refresh / toast
                              │
                    （可选）总线桥接队列/任务
                              ▼
              app_rs485 / app_modbus_* ──► 现场设备（灯/窗帘/继电器…）

AHT20 ──ui_bg_task──► hub_model.indoor_c / rh ──► 主题首页温湿度卡片
```

**原则**：设备态唯一真相是 `hub_model`；任何通道改态后刷新 UI；总线 I/O 必须在独立任务中完成。

---

## 2. 工程目录地图

```
customer_sdk/
├── README.md / AGENTS.md
├── docs/
│   ├── THEME_SECONDARY_DEV.md     # 本文
│   ├── CUSTOMER_FAQ*.md           # FAQ
│   └── api_guide/zh|en/           # ★ 分章 API + 场景指南 + AI 协作
├── CMakeLists.txt / sdkconfig.defaults / partitions.csv
├── main/
│   ├── main.c                     # 启动入口
│   ├── app_ui_theme_select.h      # ★ 主题选型（只改一行宏）
│   ├── Kconfig.projbuild          # MQTT/BLE/AHT20/RS485 等
│   ├── app/                       # ★ 开放外设 API（RS485/Modbus/…）
│   │   ├── app_api.h              # 总入口
│   │   └── examples/              # 总线桥接骨架（默认不编译）
│   ├── board/                     # ★ AHT20 驱动 + aht20_calib.h
│   ├── gui/ui_bg_task.*           # AHT20 后台轮询 → hub_model
│   ├── hub_ui/                    # 共享 UI 壳层与图标
│   └── ...
├── ui/themes/slate|sand|…         # 十套主题（默认 slate）
├── spiffs_image/icons/nt/*.png
└── components/
    ├── board_bsp/include/
    ├── hub_core/include/hub_model.h
    ├── cloud_wifi/include/
    └── bt_ctrl/include/
```

分区（`partitions.csv`）：双 OTA（各 5M）+ **storage SPIFFS 5M**。

---

## 3. 环境与第一次编译烧录

### 3.1 准备

1. 安装 **ESP-IDF 5.5.x**，导出环境（`export.ps1` / `export.sh`）。
2. **只打开** `customer_sdk/` 目录。
3. 记下串口（如 `COM5`）。

### 3.2 命令

```powershell
cd <path-to>/customer_sdk
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

主题切换后：`idf.py reconfigure && idf.py build`。

### 3.3 必须刷 SPIFFS

正常 `idf.py flash` 会写入 `storage`。若只刷 app →「有字无图标」。

### 3.4 menuconfig 常用项（`main/Kconfig.projbuild`）

| 配置 | 默认倾向 | 含义 |
|------|----------|------|
| `APP_FEATURE_MQTT` | y | 巴法云 MQTT |
| `APP_FEATURE_BLE` / `MESH` | y | 蓝牙 / Mesh |
| `AHT20_ENABLE` | y | 板载/外插 AHT20，后台写入 `hub_model` |
| `APP_ENABLE_RS485` | y | 编译 RS485/Modbus 开放 API |
| `APP_RS485_AUTO_INIT` | **n** | 启动时自动 `app_rs485_init`（确认无 UART 冲突后再开） |
| `UI_AMBIENT_*` | — | 待机降背光 |

LVGL PNG + POSIX FS 已在 `sdkconfig.defaults` 打开，勿随意关掉。

---

## 4. 十套 Hub 主题

### 4.1 能力边界

- 十套主题**共用** `main/hub_ui` 页面逻辑与路由。
- 各主题只换配色、布局与入口构图。
- **不提供** SquareLine `default` 主题包。

### 4.2 一览

| 宏 | 目录 | 说明 |
|---|---|---|
| `SLATE` | `ui/themes/slate/` | 深灰商务（默认） |
| `SAND` … `METRO` | `ui/themes/<id>/` | 其余九套，见 `app_ui_theme_select.h` |

主题目录通常含：`boot.c`、`palette.c`、`theme_local.*`、`home.c`、`pages_*.c`。

### 4.3 切换主题

编辑 `main/app_ui_theme_select.h`：

```c
#define APP_UI_THEME_ID APP_UI_THEME_SLATE
```

然后 `idf.py reconfigure && idf.py build && idf.py -p COMx flash`。

### 4.4 温湿度在主题的哪个位置？

以默认 **Slate** 为例：首页中间一排 metrics 卡片为 **功率 / 温度 / 湿度**，读的是：

- `hub_model()->indoor_c`
- `hub_model()->rh`

实现文件：`ui/themes/slate/home.c`。  
其它主题（ocean / forest / ink…）同样绑定 `indoor_c` / `rh`，布局文案略有不同。

数据未就绪（尚未采到 AHT20）时，Slate 显示 `--.-°C` / `--%`，避免把模型演示默认值当成真值。

### 4.5 新建主题 / 改动策略

| 目标 | 改哪里 |
|------|--------|
| 只改配色/圆角 | 当前主题 `palette.c`、`theme_local.c` |
| 改某一页布局 | `home.c` / `pages_*.c` |
| 改导航、网络页逻辑 | `main/hub_ui/`（多主题抽测） |
| 改设备点位语义 | `hub_model_*`，勿绕过模型 |
| 按钮要驱动 RS485 | 回调只改 model + **入队**，见 §12 |

新建主题：复制 `ui/themes/_template/` → 登记 `app_ui_theme_select.h` + `resolve_ui_theme.cmake`。

---

## 5. 图标与 SPIFFS

1. 文件：`spiffs_image/icons/nt/<name>.png`
2. 运行时：`S:/icons/nt/<name>.png`（`hub_icons.c`）
3. 依赖：`CONFIG_APP_USE_SPIFFS_UI_ASSETS` + `LV_USE_PNG` + `LV_USE_FS_POSIX`（letter=`S`）

缺 PNG/FS/未刷 storage → 图标全空。日志核对：`spiffs mounted`、`hub_ico: PSRAM icon cache xx/35`。

---

## 6. 业务模型 API（`hub_model`）

头文件：`components/hub_core/include/hub_model.h`。

```c
#include "hub_model.h"
#include "hub_ui.h"

hub_model_t *m = hub_model();
hub_model_set_room(0);
hub_model_apply_scene("home");
hub_model_toggle_widget(room, slot);
hub_model_set_widget_level(room, slot, 80);
hub_model_toast("已执行");
hub_ui_refresh();
```

跨线程改 LVGL：**必须** `gui_task_post_lvgl(...)`（见 `main/gui/gui_task.h`），禁止在 Wi‑Fi/总线任务里直接 `lv_*`。

---

## 7. Wi‑Fi 联网

头文件：`wifi_management.h`。另有开放层排序扫描：`app_wifi_util.h`（`app_wifi_scan_sorted`）。

屏上网络页已对接；自研逻辑勿第二套 `esp_wifi_init`。

---

## 8. MQTT 云控（巴法云）

头文件：`wifi_bemfa_client.h`。需 `APP_FEATURE_MQTT=y`。

本地改态后建议：`hub_model_*` → `hub_ui_refresh()` → `wifi_bemfa_client_schedule_sync()`。  
若同时控 RS485：**云端与触摸必须共用同一总线桥接**（见 §12 / G01），避免状态分裂。

---

## 9. 蓝牙（BLE / Mesh）

头文件：`bt_management.h`、`bt_proto.h`。  
扫描前后调用 `app_coexist_before/after_ble_scan()`，减轻与 CH390 SPI 冲突。  
点位可覆盖 `bt_management_apply_set_state`，内部改 `hub_model` 并可入队 Modbus 写出。

---

## 10. 以太网（CH390）

头文件：`board_ethernet_ch390.h`。  
`app_rs485_write` 内部已通过 `app_coexist_*_rs485_burst` 暂停 ETH SPI，降低总线突发时丢包概率。

---

## 11. AHT20 温湿度：显示、校准、排障（重点）

### 11.1 数据通路（已接好）

```
AHT20 (I2C 与触摸共用)
    │  ui_bg_task 约 10s 轮询（CONFIG_AHT20_ENABLE=y）
    ▼
aht20_read()  ← 已应用 aht20_calib.h
    ▼
ui_runtime_indoor_apply_climate(temp, rh)
    ▼
hub_model()->indoor_c  /  hub_model()->rh
    ▼
hub_ui_refresh() → 主题首页温度/湿度卡片
```

相关源码：

| 文件 | 作用 |
|------|------|
| `main/gui/ui_bg_task.c` | 后台 init/轮询，成功后投递 LVGL |
| `main/hub_ui/ui_runtime_stubs.c` | 写入 `hub_model` 并 refresh |
| `main/board/aht20.c` | 驱动（开放） |
| `main/board/aht20_calib.h` | **★ 客户校准宏** |
| `ui/themes/slate/home.c` | 首页显示（默认主题示例） |

日志关键字：`AHT20 ready (bg task)`；长期失败会退避重试（板子可不焊 AHT20）。

### 11.2 现场示值校准（客户最常做）

编辑 **`main/board/aht20_calib.h`**，公式：

```text
温度_out(°C) = 温度_raw × AHT20_TEMP_SCALE + AHT20_TEMP_OFFSET_C
湿度_out(%)  = 湿度_raw × AHT20_RH_SCALE   + AHT20_RH_OFFSET_PCT
```

| 宏 | 默认 | 用法 |
|----|------|------|
| `AHT20_CALIB_ENABLE` | `1` | `0` = 关闭校准 |
| `AHT20_TEMP_OFFSET_C` | `0.0f` | 偏高则填负数，如 `-1.5f` |
| `AHT20_TEMP_SCALE` | `1.0f` | 增益微调 |
| `AHT20_RH_OFFSET_PCT` | `0.0f` | 湿度偏移 |
| `AHT20_RH_SCALE` | `1.0f` | 湿度增益 |
| `AHT20_RH_CLAMP` | `1` | 校准后湿度夹紧 0..100 |

示例：

```c
/* main/board/aht20_calib.h */
#define AHT20_TEMP_OFFSET_C   (-1.5f)   /* 屏上偏高 1.5℃ */
#define AHT20_RH_OFFSET_PCT   (3.0f)    /* 湿度偏低 3% */
```

然后：

```powershell
idf.py build
idf.py -p COMx flash monitor
```

调参建议：

1. 用 `aht20_read_raw` 对照标准表读原始值  
2. 改 OFFSET/SCALE  
3. 烧录后看首页示值（同源：`aht20_read` 路径）  
4. **不要在主题里再减一遍偏移**（校准只在驱动层做一次）

### 11.3 自读 API

```c
#include "aht20.h"
#include "withthewind_board_lvgl_init.h"

aht20_init(board_i2c_get_handle());
float t = 0, rh = 0;
aht20_read(&t, &rh);       /* 校准后 */
aht20_read_raw(&t, &rh);   /* 未校准，调 OFFSET 时对照用 */
```

是否已有有效采样：`ui_bg_task_indoor_ready()`（`main/gui/ui_bg_task.h`）。

### 11.4 排障

| 现象 | 排查 |
|------|------|
| 首页一直 `--` | 模块未焊/方向错、`AHT20_ENABLE`、I2C 冲突、日志无 `AHT20 ready` |
| 有数但和标准表差固定量 | 改 `aht20_calib.h` OFFSET |
| 改宏无效 | 是否重新编译烧录；主题是否误改另一套状态 |
| 触摸异常 | AHT20 与 GT911 同总线，检查接线与地址 `0x38` |

更细 FAQ：[CUSTOMER_FAQ.md §1.13](./CUSTOMER_FAQ.md)。

---

## 12. RS485 / Modbus 控制（重点）

开放层已提供测试台验证过的半双工 + Modbus 薄客户端，**无需再从零写 DE 时序**。

### 12.1 API 一览

```c
#include "app_api.h"

app_rs485_init(NULL);   /* 或传入自定义 app_rs485_config_t */
uint16_t reg = 0;
esp_err_t e = app_modbus_read_holding(0x01, 0x0000, 1, &reg, 1, 700);
e = app_modbus_write_single(0x01, 0x0000, 0x0001, 700);
app_rs485_update_hub_health(e == ESP_OK, e == ESP_OK ? 95 : 20, true);
```

| API | 头文件 | 作用 |
|-----|--------|------|
| `app_rs485_init/write/read/transact` | `app_rs485.h` | 半双工 UART + DE |
| `app_modbus_read_holding` (FC03) | `app_modbus_rtu.h` | 读保持寄存器 |
| `app_modbus_write_single` (FC06) | `app_modbus_rtu.h` | 写单寄存器 |
| `app_gpio_out_set` | `app_gpio_out.h` | DE/RE（高=发送） |
| `app_coexist_*_rs485_burst` | `app_coexist.h` | 写突发时 pause ETH（write 内已调） |
| `app_beep_pulse` / `click_if_enabled` | `app_beep.h` | 操作反馈音 |

默认硬件：`UART0` · TX **GPIO43** · RX **GPIO44** · 115200 8N1 · DE=`app_gpio_out_set`。

### 12.2 必读：控制台 UART 冲突

若 `sdkconfig` 里控制台也是 UART0，与默认 RS485 **冲突**。任选其一：

1. menuconfig → 控制台改为 **USB Serial/JTAG**（推荐，与验证环境一致）  
2. 或自定义：

```c
app_rs485_config_t cfg;
app_rs485_get_default_config(&cfg);
cfg.uart_num = UART_NUM_1;
cfg.tx_gpio = /* 向硬件方确认 */;
cfg.rx_gpio = /* ... */;
cfg.baud = 9600;
app_rs485_init(&cfg);
```

`APP_RS485_AUTO_INIT` 默认 **关闭**；冲突解决后再打开或在 `main.c` 手动 `init`。

### 12.3 线程模型（做错会卡触摸）

| 线程 | 允许 | 禁止 |
|------|------|------|
| LVGL / 按钮回调 | `hub_model_*`、`hub_ui_refresh`、`QueueSend` 意图 | `app_modbus_*` / 长时间 `read` |
| 总线任务 | `app_rs485_*` / `app_modbus_*` | 直接 `lv_*` |
| 回写 UI | `gui_task_post_lvgl` 或 `update_hub_health(..., true)` | — |

### 12.4 推荐接法：总线桥接（UI → Modbus）

完整步骤见 **[G01](./api_guide/zh/guides/G01_modbus_rs485_lvgl.md)**。摘要：

1. 无 UI 先证明 `app_modbus_read_holding` 能通  
2. 复制示例：

```
main/app/examples/app_bus_bridge_example.c  →  main/app/app_bus_bridge.c
main/app/examples/app_bus_bridge_example.h  →  main/app/app_bus_bridge.h
```

3. 把 `.c` 加入 `main/CMakeLists.txt` 的 `MAIN_SRCS`  
4. `gui_task_init()` 之后 `app_bus_bridge_start()`  
5. 主题按钮：

```c
hub_model_toggle_widget(room, slot);
hub_ui_refresh();
app_beep_click_if_enabled();
app_bus_bridge_post_write(slave, reg_addr, on ? 1 : 0);  /* 非阻塞入队 */
```

6. （可选）云端命令也调用同一个 `post_write`，保证手机/屏/总线一致  

### 12.5 扩展更多功能码

FC05/0F/10：用 `app_modbus_crc16` + `app_rs485_transact` 自组帧；或引入 esp-modbus 但仍占同一 UART（勿与本薄封装并发混用）。

### 12.6 排障

| 现象 | 排查 |
|------|------|
| 无应答 | DE 极性、A/B、终端电阻、从站地址、波特率 |
| init 后日志乱码/死机 | UART0 与控制台冲突 |
| 触摸卡顿 | 总线调用跑在 LVGL 线程 |
| ETH 掉线 | 确认走 `app_rs485_write`（含 coexist pause） |
| 链接缺 expander 符号 | `board_io_expander.h` 客户子集 + 链接 `board_bsp.a` |

分章详情：[RS485](./api_guide/zh/10_gpio_rs485.md) · [Modbus](./api_guide/zh/11_modbus.md) · [控制通路](./api_guide/zh/19_control_map.md)

---

## 13. 端到端控制对照表

| 入口 | 建议数据流 |
|------|-----------|
| 触摸 UI | `hub_model_*` →（可选）`app_bus_bridge_post_write` → Modbus |
| 巴法 MQTT | 库内点位 → **同一桥接**写出 → `schedule_sync` |
| BLE SET_STATE | `apply_set_state` → model → 同一桥接 |
| 以太网 TCP | 自研解析 → `app_modbus_*` → 刷新 model |
| 场景一键 | `apply_scene` → 批量队列写 |
| AHT20 | `ui_bg_task` → `indoor_c`/`rh` → 主题刷新（只读传感） |

产品方向选型：[G00](./api_guide/zh/guides/G00_directions.md)

---

## 14. 二次开发自检清单

- [ ] 工程目录是 `customer_sdk/`，目标 `esp32s3`，IDF 5.5.x  
- [ ] `APP_UI_THEME_ID` 为十套 Hub 之一  
- [ ] `flash` 含 storage；`hub_ico` 缓存数量正常  
- [ ] AHT20：日志 `AHT20 ready`；首页温度/湿度有真实数；偏差已用 `aht20_calib.h` 校准  
- [ ] RS485：UART 冲突已处理；`read_holding` 无 UI 可通；按钮只入队不阻塞  
- [ ] Wi‑Fi / MQTT / BLE 按需联调；多入口共用 `hub_model`  
- [ ] 未修改 / 未依赖反汇编 `.a`  

---

## 15. 常见问题

**Q：有界面但图标全空？**  
PNG + POSIX FS + 已 flash storage + SPIFFS 内有 `icons/nt/*.png`。

**Q：首页温湿度一直是 `--`？**  
查模块焊接、`AHT20_ENABLE`、日志 `AHT20 ready`、是否新固件。

**Q：温湿度和标准表差固定值？**  
改 `main/board/aht20_calib.h` 的 OFFSET/SCALE 后重新烧录；勿在主题再减一次。

**Q：RS485 头文件在哪？**  
`main/app/app_rs485.h`、`app_modbus_rtu.h`，或 `#include "app_api.h"`。

**Q：点按钮卡死？**  
把 `app_modbus_*` 挪出 LVGL 回调，用 §12.4 桥接。

**Q：改了主题宏仍是旧主题？**  
`idf.py reconfigure`；必要时 `fullclean`。

**Q：能否改背光 GPIO？**  
引脚在 `board_bsp.a` 内；公开 API 只提供亮度百分比。换板需厂商重出 BSP。

**Q：MQTT UID/主题能改吗？**  
在封闭 `cloud_wifi.a`；无配置接口时联系厂商或自建 MQTT 客户端。

**Q：链接缺 `bt_switch_control_linker_keep`？**  
确认 `main/hub_ui/ui_runtime_stubs.c` 已编入（内含该符号）。

**Q：CMake 报 Missing required kconfig（LIBJPEG/LIBPNG/LZ4）？**  
见工程根 `tools/pin_esp_lvgl_port_lvgl.py`（配置时自动把 esp_lvgl_port 的 lvgl 钉到 8.4.*）。失败后再跑一次 `idf.py reconfigure`。

---

## 16. 相关路径速查

```
customer_sdk/
  docs/THEME_SECONDARY_DEV.md
  docs/api_guide/zh/                 # 分章 + G00–G06
  main/app_ui_theme_select.h
  main/app/app_api.h                 # 开放外设总入口
  main/app/examples/                 # 总线桥接示例
  main/board/aht20_calib.h           # ★ 温湿度校准
  main/gui/ui_bg_task.c              # AHT20 轮询
  main/hub_ui/                       # 壳层 / 图标 / ui_runtime 桩
  ui/themes/slate/home.c             # 首页温湿度显示（默认主题）
  spiffs_image/icons/nt/
  components/hub_core/include/hub_model.h
  components/cloud_wifi/include/
  components/bt_ctrl/include/
  components/board_bsp/include/aht20.h
```

---

## 17. 建议学习顺序

1. 编译烧录默认 `SLATE`，确认图标、触摸、首页温湿度（有模块时）。  
2. 只改 `palette.c` 看配色；对照标准表调 `aht20_calib.h`。  
3. 读 `hub_model.h`，按钮回调改 widget + `hub_ui_refresh()`。  
4. 无 UI 跑通 `app_modbus_read_holding` → 接入 `examples` 总线桥接（[G01](./api_guide/zh/guides/G01_modbus_rs485_lvgl.md)）。  
5. 联调 Wi‑Fi +（可选）巴法 MQTT，云端与触摸共用桥接。  
6. 按需开 BLE/Mesh / 以太网网关（[G00](./api_guide/zh/guides/G00_directions.md)）。  

完成以上步骤后，即可在不接触封闭源码的前提下，完成 UI 定制、温湿度校准与 RS485 现场控制扩展。

使用 AI 协助时：置顶 [AI_CONTEXT.md](./api_guide/zh/AI_CONTEXT.md)，工作流见 [AI_DEV.md](./api_guide/zh/AI_DEV.md)。
