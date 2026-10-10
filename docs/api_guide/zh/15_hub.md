# 15. Hub 模型与 UI（详细）

> `hub_model` 是屏上设备态的**唯一真相**。主题只负责「长什么样」；开关语义一律走模型 API。

---

## 1. 核心头文件

| 头文件 | 用途 |
|--------|------|
| `hub_model.h` | 房间 / widget / 场景 / 设置 / toast |
| `hub_ui.h` | `hub_ui_init/go/refresh`、路由 |
| `hub_theme.h` | 主题钩子 |
| `hub_device_ui.h` | 网络页、亮度 |
| `hub_icons.h` | SPIFFS 图标 |

---

## 2. 常用模型 API

```c
hub_model_t *m = hub_model();

hub_model_set_room(0);
hub_model_apply_scene("home");
hub_model_toggle_widget(room, slot);
hub_model_step_widget(room, slot, +5);
hub_model_set_widget_level(room, slot, 80);
hub_model_toast("已执行");
hub_ui_refresh();   /* 模型变更后刷新当前页 */
```

| 类型 `hub_wtype_t` | 典型用途 |
|--------------------|----------|
| `HUB_W_ONOFF` | 灯/继电器 |
| `HUB_W_DIMMER` | 调光 |
| `HUB_W_CURTAIN` / `SHUTTER` | 窗帘/卷帘 |
| `HUB_W_CLIM` / `THERMO` | 空调/地暖 |
| `HUB_W_PLUG` / `FAN` | 插座/风机 |

---

## 3. 主题二次开发

1. 切换：`main/app_ui_theme_select.h` → `APP_UI_THEME_ID`  
2. 改配色：`ui/themes/<id>/palette.c`  
3. 改布局：`home.c` / `pages_*.c`  
4. 必实现：`hub_theme_build(parent, route)`  

按钮回调模板（接总线时）：

```c
hub_model_toggle_widget(room, slot);
hub_ui_refresh();
app_beep_click_if_enabled();
app_bus_bridge_post_write(slave, addr, on ? 1 : 0);  /* 见 examples */
```

---

## 4. 与其它技术融合

| 通道 | 应如何更新 UI |
|------|----------------|
| 触摸 | 直接 `hub_model_*` + `hub_ui_refresh` |
| Modbus 回读任务 | 改 model + `gui_task_post_lvgl(refresh)` |
| MQTT / BLE | 改 model + refresh +（可选）写出总线 |

专题：[二次开发入门](./00_secondary_dev.md) · [G01](./guides/G01_modbus_rs485_lvgl.md) · [G00 方向](./guides/G00_directions.md)
