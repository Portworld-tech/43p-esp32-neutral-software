# G03. Scenario: BLE / Mesh near-field control

## User story

A mini-program or provisioner sends SET_STATE nearby; the panel updates; optionally the same command is written to a Modbus slave.

## Technologies

`bt_management` · `bt_proto` · `app_coexist` · `hub_model` · (optional) RS485

## Steps

1. Enable `APP_FEATURE_BLE` / `MESH` as needed  
2. Override the weak symbol:

```c
esp_err_t bt_management_apply_set_state(uint8_t item_id, uint8_t value)
{
    /* Map item_id → room/slot */
    hub_model_set_widget_level(0, 0, value ? 100 : 0);
    hub_ui_refresh();
    app_bus_bridge_post_write(1, 0x0000, value); /* optional */
    return ESP_OK;
}
```

3. Around BLE scans:

```c
app_coexist_before_ble_scan();
bt_management_ble_scan_start(5);
/* … */
app_coexist_after_ble_scan();
```

## See also

[Bluetooth](../06_bt.md) · [Coexistence](../12_coexist.md) · [G01](./G01_modbus_rs485_lvgl.md)
