# G04. Scenario: Ethernet + RS485 gateway

## User story

The panel has Ethernet uplink; you add TCP/HTTP northbound; Modbus RTU southbound; the ops page shows ETH / RS485 health.

## Technologies

`board_ethernet_ch390_*` · `app_coexist` · `app_rs485` / `app_modbus_*` · `hub_model.protos[]` · `gui_task`

## Steps

1. Confirm `board_ethernet_ch390_init` + `try_start` and a valid `get_ip`  
2. Before heavy ETH work: `app_coexist_before_ethernet_work()`  
3. In open `main/`, add a TCP server task; after parse call `app_modbus_write_single`  
4. Periodically update protocol health:

```c
bool eth_ok = board_ethernet_ch390_link_up();
/* update hub_model()->protos[i] */
app_rs485_update_hub_health(rs485_ok, health, true);
```

5. `app_rs485_write` already pauses CH390 SPI during bursts  

## See also

[Ethernet](../07_eth.md) · [RS485](../10_gpio_rs485.md) · [Coexistence](../12_coexist.md)
