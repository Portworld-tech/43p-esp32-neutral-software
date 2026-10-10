# G04. 场景：以太网 + RS485 网关

## 用户故事

网线接入中控；对上提供 TCP/HTTP；对下 Modbus RTU；运维页显示 ETH / RS485 健康度。

## 融合技术

`board_ethernet_ch390_*` · `app_coexist` · `app_rs485` / `app_modbus_*` · `hub_model.protos[]` · `gui_task`

## 步骤

1. 确认 `board_ethernet_ch390_init` + `try_start`，`get_ip` 有地址  
2. 重负载前：`app_coexist_before_ethernet_work()`  
3. 在 `main/` 自研 TCP server 任务（开放层），解析后调用 `app_modbus_write_single`  
4. 周期性：

```c
bool eth_ok = board_ethernet_ch390_link_up();
/* 更新 hub_model()->protos[i] */
app_rs485_update_hub_health(rs485_ok, health, true);
```

5. RS485 突发时 `app_rs485_write` 已自动 pause CH390 SPI  

## 相关

[以太网](../07_eth.md) · [RS485](../10_gpio_rs485.md) · [共存](../12_coexist.md)
