# Customer SDK 二次开发总览（从零到量产定制）

本文面向拿到 `customer_sdk/` 的客户工程师：从工程是什么、能改什么、怎么编译烧录，到主题/图标、Hub 业务模型、Wi‑Fi、巴法云 MQTT、蓝牙，以及如何自行扩展 **RS485 / UART 总线控制**。请把本目录当作**独立 ESP-IDF 工程**使用，不要与厂商私有产品仓根目录混开。

---

## 0. 五分钟结论

| 问题 | 答案 |
|------|------|
| 这是什么？ | ESP32-S3 + LVGL 智能中控固件的**分层客户包**：板级/云/蓝牙/业务模型以预编译库交付，UI 主题与资源开放 |
| 目标芯片 | `esp32s3`（预编译 `.a` 仅此目标） |
| IDF 版本 | **ESP-IDF 5.5.x**（与交付说明一致） |
| 默认 UI | 十套 Hub 主题之一（默认建议 `SLATE`），**不含** SquareLine `default` 主题 |
| 怎么控设备？ | UI → `hub_model_*`；云端 → 巴法 MQTT；近场 → BLE/Mesh；有线网 → CH390 以太网；RS485 **需自行在开放层接 UART** |
| 烧录注意 | 必须刷入 **storage（SPIFFS）**，否则图标空白 |

---

## 1. 产品与软件架构

### 1.1 硬件能力（板级已封装）

典型交付板（Withthewind 类）：

- MCU：ESP32-S3
- 显示：ST7701 RGB **480×480**
- 触摸：GT911
- IO 扩展：NCA9555（蜂鸣等部分外设）
- 背光：MCU **GPIO4 LEDC** 软件 PWM（已封进 `board_bsp.a`）
- 可选：AHT20 温湿度、CH390 SPI 以太网

**引脚表与扩展器网络不在客户包内公开**；请通过下列公开头文件操作能力，而不是直接改 GPIO 表：

- `components/board_bsp/include/withthewind_board_lvgl_init.h`
- `components/board_bsp/include/board_io_expander.h`
- `components/board_bsp/include/board_ethernet_ch390.h`
- `components/board_bsp/include/aht20.h`

### 1.2 分层模型（开放 / 封闭）

```
┌─────────────────────────────────────────────────────────┐
│  开放层（源码可改）                                        │
│  ui/themes/* · main/hub_ui · spiffs_image · main/main.c │
└──────────────────────────▲──────────────────────────────┘
                           │ 调用公开 API
┌──────────────────────────┴──────────────────────────────┐
│  封闭层（仅 .a + include/*.h）                             │
│  board_bsp · hub_core · cloud_wifi · bt_ctrl              │
└─────────────────────────────────────────────────────────┘
                           │
                    ESP-IDF / LVGL / 驱动组件
```

| 开放（可改） | 封闭（`.a`，勿改、勿反汇编依赖） |
|---|---|
| `ui/themes/<主题>/` | `components/board_bsp`（屏/触摸/背光/以太网驱动封装） |
| `main/hub_ui/`（壳层、导航、图标加载、设备页） | `components/hub_core`（`hub_model` 实现） |
| `main/app_ui_theme_select.h` | `components/cloud_wifi`（Wi‑Fi + 巴法 MQTT） |
| `spiffs_image/` | `components/bt_ctrl`（BLE / Mesh 控制栈） |
| `main/main.c`、`main/app/`、`main/gui/` 等应用胶水 | |

封闭库路径示例：`components/*/lib/esp32s3/*.a`。

### 1.3 运行时数据流（控制从哪进、到哪出）

```
手机 App / 巴法云 ──MQTT──► cloud_wifi ──► bt_management / UI 状态
手机 BLE / Mesh   ────────► bt_ctrl     ──► 本地开关点位
触摸屏 UI         ────────► hub_model_* ──► 刷新界面 / toast
自研 RS485 任务   ────────►（客户代码）──► hub_model_* 或 UART 帧
```

Hub 界面的「房间 / 场景 / 协议健康度」等，优先读写 **`hub_model.h`**，不要在主题里复制一套私有状态机。

---

## 2. 工程目录地图

```
customer_sdk/
├── README.md                      # 入口说明
├── docs/
│   ├── THEME_SECONDARY_DEV.md     # 本文（总览 + 主题 + 总线扩展）
│   └── LAYERED_SDK.md             # 分层交付摘要
├── CMakeLists.txt / sdkconfig.defaults / partitions.csv
├── main/
│   ├── main.c                     # 启动入口
│   ├── app_ui_theme_select.h      # ★ 主题选型（只改一行宏）
│   ├── Kconfig.projbuild          # MQTT/BLE/Mesh/环境光等开关
│   ├── hub_ui/                    # ★ 共享 UI 壳层与图标
│   ├── cmake/resolve_ui_theme.cmake
│   └── ...
├── ui/themes/
│   ├── _template/                 # 新建主题模板
│   ├── slate|sand|ink|...         # 十套主题源码
├── spiffs_image/
│   └── icons/nt/*.png             # ★ Hub 图标资源
└── components/
    ├── board_bsp/include/         # 板级公开 API
    ├── hub_core/include/hub_model.h
    ├── cloud_wifi/include/        # wifi_management / wifi_bemfa_client
    └── bt_ctrl/include/           # bt_management / bt_proto / GATT
```

分区（`partitions.csv`）：双 OTA（各 5M）+ **storage SPIFFS 5M**。图标与自定义 JSON 等放在 SPIFFS。

---

## 3. 环境与第一次编译烧录

### 3.1 准备

1. 安装 **ESP-IDF 5.5.x**，导出环境（`export.ps1` / `export.sh`）。
2. 用 IDE 或终端**只打开** `customer_sdk/` 目录。
3. USB 串口驱动正常，记下端口号（如 `COM5`）。

### 3.2 命令

```powershell
cd <path-to>/customer_sdk
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

主题切换后需：

```powershell
idf.py reconfigure
idf.py build
```

### 3.3 必须刷 SPIFFS

工程使用 `spiffs_create_partition_image(... FLASH_IN_PROJECT)`，正常 `idf.py flash` 会写入 `storage`。若只刷 app 分区，会出现「有字/有布局、图标全空」。

### 3.4 menuconfig 常用项（`main/Kconfig.projbuild`）

| 配置 | 含义 |
|------|------|
| `APP_FEATURE_MQTT` | 启用巴法云 MQTT 客户端 |
| `APP_FEATURE_BLE` | 启用 BLE |
| `APP_FEATURE_MESH` | 启用 BLE Mesh（依赖 BLE） |
| `UI_AMBIENT_ENABLE` | 环境光/待机相关 |
| `AHT20_ENABLE` | 板载温湿度 |

LVGL 侧默认已在 `sdkconfig.defaults` 打开 PNG + POSIX FS（见第 5 节），勿随意关掉。

---

## 4. 十套 Hub 主题

### 4.1 能力边界（再强调）

- 十套主题**共用** `main/hub_ui` 页面逻辑与路由。
- 各主题只换配色、布局微调和入口路由实现。
- **不提供** SquareLine 的 `default` 主题包。

### 4.2 一览

| 宏 `APP_UI_THEME_*` | 目录 | 说明 |
|---|---|---|
| `SLATE` | `ui/themes/slate/` | 深灰商务（推荐默认） |
| `SAND` | `ui/themes/sand/` | 暖沙 |
| `INK` | `ui/themes/ink/` | 墨色 |
| `FOREST` | `ui/themes/forest/` | 森林绿 |
| `DUSK` | `ui/themes/dusk/` | 暮色 |
| `OCEAN` | `ui/themes/ocean/` | 海洋蓝 |
| `ZEN` | `ui/themes/zen/` | 禅意浅色 |
| `PULSE` | `ui/themes/pulse/` | 动感高对比 |
| `BLOOM` | `ui/themes/bloom/` | 花卉暖调 |
| `METRO` | `ui/themes/metro/` | 磁贴风 |

每个主题目录通常包含：

| 文件 | 作用 |
|---|---|
| `boot.c` | `app_ui_start()` → `hub_ui_init()` |
| `palette.c` / `theme_local.*` | 颜色与局部样式、按键回调 |
| `home.c` | 首页构图 |
| `pages_room.c` / `pages_scenes.c` / `pages_ops.c` / `pages_life.c` | 业务页构图 |

### 4.3 切换主题（必做）

编辑 `main/app_ui_theme_select.h`，**只保留一个**有效 ID，例如：

```c
#define APP_UI_THEME_ID APP_UI_THEME_OCEAN
```

然后 `idf.py reconfigure && idf.py build && idf.py -p COMx flash`。

### 4.4 新建主题

1. 复制 `ui/themes/_template/` 为新目录名。
2. 在 `app_ui_theme_select.h` 增加宏与 ID。
3. 在 `main/cmake/resolve_ui_theme.cmake` 合法列表中登记。
4. 改 palette / pages，业务状态仍走 `hub_model_*`。

### 4.5 推荐改动策略

| 目标 | 改哪里 |
|------|--------|
| 只改配色/圆角/间距 | 当前主题 `palette.c`、`theme_local.c` |
| 改某一页布局 | 主题内 `home.c` / `pages_*.c` |
| 改导航、待机、网络设置页逻辑 | `main/hub_ui/`（改后建议多主题抽测） |
| 改设备点位语义 | 调用 `hub_model.h` API，勿绕过模型硬编码 |

路由枚举见 `main/hub_ui/hub_ui.h`（`HUB_ROUTE_HOME` … `HUB_ROUTE_STANDBY`）。主题包实现 `hub_theme_build(parent, route)`。

---

## 5. 图标与 SPIFFS（与产品主工程对齐）

Hub 图标不走 SquareLine 位图：

1. 文件：`spiffs_image/icons/nt/<name>.png`
2. 运行时路径：`S:/icons/nt/<name>.png`（`main/hub_ui/hub_icons.c`）
3. 依赖配置（已写入 `sdkconfig.defaults`）：

```
CONFIG_APP_USE_SPIFFS_UI_ASSETS=y
CONFIG_LV_USE_FS_POSIX=y
CONFIG_LV_FS_POSIX_LETTER=83          # 'S'
CONFIG_LV_FS_POSIX_PATH="/spiffs"
CONFIG_LV_USE_PNG=y
CONFIG_LV_IMG_CACHE_DEF_SIZE=16
CONFIG_LV_FONT_SIMSUN_16_CJK=y
```

缺少 **PNG 解码** 或 **POSIX FS** → 骨架在、图标空（最常见坑）。

开机日志核对：

- `spiffs: ... mounted`
- `hub_ico: PSRAM icon cache xx/35 icons`（`xx` 应接近 35，而不是 0）

换图标：覆盖同名 PNG（建议透明底、约 96×96）后 `build flash`。名称与枚举对应见 `hub_icons.c` 的 `s_names[]`。

---

## 6. 业务模型 API（`hub_model`）

头文件：`components/hub_core/include/hub_model.h`。

### 6.1 核心概念

- **房间** `HUB_ROOM_COUNT`（4）：每个房间一组 widget
- **Widget 类型**：开关、调光、窗帘、卷帘、空调、地暖、风机、插座等（`hub_wtype_t`）
- **场景** `hub_model_apply_scene(scene_id)`
- **协议槽** `protos[]`：UI 上展示 Wi‑Fi / BT / RS485 / 以太网等健康度（演示与运维页用）
- **设置**：中英文、夜间模式、按键音
- **Toast**：`hub_model_toast()` / `hub_model_take_toast()`

### 6.2 常用调用（主题与自研任务均可）

```c
#include "hub_model.h"

hub_model_t *m = hub_model();

hub_model_set_room(0);
hub_model_apply_scene("home");      /* 具体 id 以模型内定义为准 */
hub_model_toggle_widget(room, slot);
hub_model_step_widget(room, slot, +1);
hub_model_set_widget_level(room, slot, 80);
hub_model_step_ac(+1);
hub_model_step_curtain(-5);
hub_model_set_armed(true);
hub_model_toast("窗帘已开启");
hub_ui_refresh();                   /* 模型变更后刷新当前页 */
```

**原则**：任何控制通道（MQTT / BLE / RS485）在改变设备态后，应更新 `hub_model`（或调用其 API），再 `hub_ui_refresh()`，保证屏上状态一致。

---

## 7. Wi‑Fi 联网

头文件：`components/cloud_wifi/include/wifi_management.h`。

| API | 作用 |
|-----|------|
| `wifi_management_foundation_init()` | NVS + netif + 默认事件循环 |
| `wifi_management_driver_init()` | 尽早 `esp_wifi_init` |
| `wifi_management_start()` | 启动 STA |
| `wifi_management_connect(ssid, pwd)` | 连接热点 |
| `wifi_management_scan_blocking(...)` | 阻塞扫描（设置页可用） |
| `wifi_management_is_connected()` | 是否已拿到 IP |
| `wifi_management_disconnect_user()` | 用户断开 |

屏上网络页已对接上述 API（`main/hub_ui/hub_device_ui.c`）。自研逻辑请同样走此接口，避免第二套 Wi‑Fi 初始化。

---

## 8. MQTT 云控（巴法云 Bemfa）

头文件：`components/cloud_wifi/include/wifi_bemfa_client.h`。

### 8.1 能力说明

封闭库内已实现：

- 连接 `mqtt://bemfa.com:9501`
- 订阅命令主题、发布状态主题
- Wi‑Fi 拿到 IP 后自动 `wifi_bemfa_client_start()`（需 `CONFIG_APP_FEATURE_MQTT=y`）
- 断网时停止客户端

公开 API：

```c
esp_err_t wifi_bemfa_client_start(void);
void wifi_bemfa_client_stop(void);
void wifi_bemfa_client_publish_status_u8(uint8_t item_id, uint8_t value, bool ok);
void wifi_bemfa_client_schedule_sync(void);  /* 合并上报全量快照 */
```

### 8.2 客户侧怎么用

1. **menuconfig** 打开 `APP_FEATURE_MQTT`。
2. 用屏上 Wi‑Fi 页连上路由器，确认日志有 MQTT 连接成功。
3. 在巴法云控制台创建与固件默认 UID/主题一致的设备（或联系厂商提供可改配的 UID/主题方案；默认常量在封闭库内）。
4. 手机端下发开关/温度步进等命令后，设备会走到内部点位控制；本地变更可用 `wifi_bemfa_client_publish_status_u8` / `schedule_sync` 回传。

### 8.3 与 Hub UI 联动建议

- 云端改状态 → 库内已尽量落到控制路径；若你扩展了新点位，在处理完后调用 `hub_model_*` + `hub_ui_refresh()` + `wifi_bemfa_client_schedule_sync()`。
- 不要在 LVGL 回调里做长时间阻塞网络；发布接口已按库设计可从多任务调用，仍需避免在 ISR 中调用。

### 8.4 自建 MQTT（非巴法）

若必须接自有 Broker：可在 **开放的 `main/`** 新增独立 `esp_mqtt` 客户端任务，订阅后调用 `hub_model_*` / `bt_management_*`。不要替换封闭的 `cloud_wifi.a`；与巴法并存时注意 topic 与 `APP_FEATURE_MQTT` 冲突，建议二选一或加你自己的 Kconfig 开关隔离。

---

## 9. 蓝牙控制（BLE / Mesh）

头文件：

- `components/bt_ctrl/include/bt_management.h`
- `components/bt_ctrl/include/bt_proto.h`
- `components/bt_ctrl/include/bt_gatt_star.h`

### 9.1 能力

- BLE GATT / 可选 BR/EDR / BLE Mesh
- 本地提交控制帧：`bt_management_local_submit_command()`
- Mesh 配网开关、链路状态查询等（详见头文件注释）

### 9.2 客户侧

1. `APP_FEATURE_BLE` / `APP_FEATURE_MESH` 按需打开。
2. 内存与共存：RGB 刷屏 + Wi‑Fi + BLE 同时开时堆紧张，优先保证产品已验证的默认配置；加功能时观察启动日志与 `wifi_management_start_failed()`。
3. UI 侧改点位可走 `bt_management_local_submit_command` 或库内 `apply_set_state` 类接口（以头文件为准），并同步 `hub_model`。

---

## 10. 以太网（CH390）

头文件：`board_ethernet_ch390.h`。

- `board_ethernet_ch390_init()`：在 `wifi_management_foundation_init()` 之后调用（产品启动流程已接好）。
- 查询：`is_ready` / `link_up` / `get_ip` / `get_link_info`。
- 共存：`board_ethernet_ch390_set_traffic_paused(true)` 可在自测重负载总线时暂停 SPI 以太网流量，降低 DMA/堆压力。

运维页可能提示「以太网未编译进固件」——以当前 `sdkconfig` 与板级宏为准。

---

## 11. 板级公开 API（背光 / 蜂鸣 / I2C）

`withthewind_board_lvgl_init.h`：

```c
esp_err_t board_display_start_with_lvgl_cfg(const lvgl_port_cfg_t *cfg);
esp_err_t board_backlight_set(int brightness_percent);
esp_err_t board_backlight_on(void);
esp_err_t board_backlight_off(void);
esp_err_t board_beep_set(int on);
i2c_master_bus_handle_t board_i2c_get_handle(void);
```

设置页亮度滑条已调用 `board_backlight_*`。自研传感器可挂在 `board_i2c_get_handle()` 上（注意地址冲突与总线占用）。

---

## 12. RS485 / Modbus（开放层已提供）

测试台验证过的半双工逻辑已移植到本工程开放层，详见 **[api_guide/zh/10_gpio_rs485.md](./api_guide/zh/10_gpio_rs485.md)**。

```c
#include "app_api.h"

app_rs485_init(NULL);   /* 默认 UART0 GPIO43/44 + 扩展器 DE；勿与 UART0 控制台冲突 */
uint16_t reg = 0;
app_modbus_read_holding(0x01, 0x0000, 1, &reg, 1, 700);
app_rs485_update_hub_health(true, 90, true);
```

| API | 头文件 |
|-----|--------|
| `app_rs485_*` | `main/app/app_rs485.h` |
| `app_modbus_*` | `main/app/app_modbus_rtu.h` |
| `app_gpio_out_*` / DE | `main/app/app_gpio_out.h` |
| `app_coexist_*_rs485_burst` | `main/app/app_coexist.h`（写总线时自动 pause ETH） |

Kconfig：`APP_ENABLE_RS485`（默认 y）、`APP_RS485_AUTO_INIT`（默认 n）。  
自定义引脚：`app_rs485_get_default_config` → 修改 → `app_rs485_init(&cfg)`。  
**禁止**在 LVGL 线程长时间阻塞 `app_rs485_read`；用独立任务 + `gui_task_post_lvgl` 回写 UI。

---

## 13. 端到端控制对照表

| 入口 | 模块 | 建议落到 |
|------|------|----------|
| 触摸 UI | `hub_ui` / 主题 `theme_local` | `hub_model_*` →（可选）RS485/GPIO |
| 巴法云手机 | `wifi_bemfa_client` | 库内点位 + 你扩展的外设写入口 |
| BLE App | `bt_management` | 协议帧 → 点位 → `hub_model` |
| 自研 MQTT | `main/` 新客户端 | 同上 |
| 定时场景 | `hub_model_run_schedule` / apply_scene | 批量改 widget 后再刷总线 |

---

## 14. 二次开发自检清单

- [ ] 工程目录是 `customer_sdk/`，目标 `esp32s3`，IDF 5.5.x  
- [ ] `APP_UI_THEME_ID` 为十套 Hub 之一（非 `DEFAULT`）  
- [ ] `flash` 含 storage；日志 `hub_ico` 缓存数量正常  
- [ ] 首页/房间/场景/运维/生活页无大块空白，中文正常  
- [ ] 背光可调，交互有蜂鸣（若硬件连接正常）  
- [ ] Wi‑Fi 可扫描连接；MQTT 开关与云端联调（若启用）  
- [ ] BLE/Mesh 按需验证（注意内存）  
- [ ] RS485：`app_rs485_init` +（可选）Modbus；独立任务、与 UI/MQTT 状态一致  

---

## 15. 常见问题

**Q：能编译，屏幕黑 / 无字？**  
确认烧录的是当前包生成的固件；日志是否有背光 LEDC 初始化。检查排线与供电。

**Q：有界面但图标全空？**  
`CONFIG_LV_USE_PNG` + `CONFIG_LV_USE_FS_POSIX`；已 flash storage；SPIFFS 内有 `icons/nt/*.png`。

**Q：改了主题宏仍是旧主题？**  
`idf.py reconfigure` 后再编译；必要时 `fullclean`。

**Q：能否用 SquareLine default？**  
本 SDK 不含 `ui/themes/default`。请用十套 Hub 或自建 hub 风格主题包。

**Q：能否改 GPIO / 背光引脚？**  
引脚在 `board_bsp.a` 内。公开 API 只提供亮度百分比等；换板需厂商重出 BSP 库。

**Q：RS485 头文件在哪？**  
`main/app/app_rs485.h`（及 `app_modbus_rtu.h`）。详见第 12 节与 `docs/api_guide/`。

**Q：MQTT UID/主题能改吗？**  
实现位于封闭 `cloud_wifi.a`。若交付未提供配置接口，请联系厂商定制库或在开放层另建 MQTT 客户端。

**Q：预编译库链接报缺符号？**  
保持 `main/CMakeLists.txt` 对 `board_bsp` / `hub_core` / `cloud_wifi` / `bt_ctrl` 的依赖；勿删 `ui_runtime` 等桩文件；目标必须是 `esp32s3`。

---

## 16. 相关路径速查

```
customer_sdk/
  docs/THEME_SECONDARY_DEV.md      # 本文
  main/app_ui_theme_select.h       # 主题开关
  main/hub_ui/                     # 共享壳层与图标
  ui/themes/<id>/                  # 主题源码
  spiffs_image/icons/nt/           # 图标 PNG
  components/hub_core/include/hub_model.h
  components/cloud_wifi/include/wifi_management.h
  components/cloud_wifi/include/wifi_bemfa_client.h
  components/bt_ctrl/include/bt_management.h
  components/board_bsp/include/withthewind_board_lvgl_init.h
  components/board_bsp/include/board_ethernet_ch390.h
  sdkconfig.defaults               # PNG/FS 等默认项
  partitions.csv                   # OTA + SPIFFS
```

---

## 17. 建议学习顺序

1. 编译烧录默认 `SLATE`，确认图标与触摸正常。  
2. 只改 `palette.c` 看配色生效。  
3. 读 `hub_model.h`，在某个按键回调里改 widget 并 `hub_ui_refresh()`。  
4. 联调 Wi‑Fi +（可选）巴法 MQTT。  
5. 按第 12 节接入真实 RS485 窗帘/空调执行器。  
6. 需要近场再开 BLE/Mesh。  

完成以上步骤后，即可在不接触封闭源码的前提下，覆盖 UI 定制与主流控制通路扩展。
