# G03. 场景：BLE / Mesh 近场控制

## 用户故事

手机小程序或配网器近场下发 SET_STATE；屏上状态更新；可选再写出到 Modbus 从站。

## 融合技术

`bt_management` · `bt_proto` · `app_coexist` · `hub_model` ·（可选）RS485

## 步骤

1. `APP_FEATURE_BLE` / `MESH` 按需打开  
2. 覆盖弱符号：

```c
esp_err_t bt_management_apply_set_state(uint8_t item_id, uint8_t value)
{
    /* item_id → room/slot 映射表 */
    hub_model_set_widget_level(0, 0, value ? 100 : 0);
    hub_ui_refresh();
    bus_bridge_post_write(1, 0x0000, value, 0, 0); /* 可选 */
    return ESP_OK;
}
```

3. BLE 扫描：

```c
app_coexist_before_ble_scan();
bt_management_ble_scan_start(5);
/* … */
app_coexist_after_ble_scan();
```

## 相关

[蓝牙](../06_bt.md) · [共存](../12_coexist.md) · [G01](./G01_modbus_rs485_lvgl.md)
