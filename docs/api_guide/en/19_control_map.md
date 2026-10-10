# 19. Control map

All channels should converge on **`hub_model`**, then optionally write the bus and/or sync to the cloud.

| Entry | Suggested flow |
|-------|----------------|
| Touch UI | `hub_model_*` → (optional) `app_bus_bridge_post_write` → Modbus |
| Bemfa MQTT | In-library points → same bridge write-out → `schedule_sync` |
| BLE SET_STATE | `apply_set_state` → model → same bridge |
| Ethernet TCP | Your parser → `app_modbus_*` → refresh model |
| One-tap scene | `apply_scene` → batched queue writes |

```text
Touch / MQTT / BLE / ETH / Scene
              │
              ▼
         hub_model   ←── single truth
              │
     ┌────────┼────────┐
     ▼        ▼        ▼
 hub_ui   bus_bridge  MQTT sync
 refresh   Modbus      schedule_sync
```

**Pick a product shape** → [G00](./guides/G00_directions.md)  
**Panel → RS485 walkthrough** → [G01](./guides/G01_modbus_rs485_lvgl.md)

---

[← Kconfig](./18_kconfig.md) | [README](./README.md) | [FAQ →](./20_faq.md)
