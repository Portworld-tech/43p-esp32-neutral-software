#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize CH390 (SPI), attach esp_netif, register ETH GOT_IP handler.
 * No-op / ESP_OK when BOARD_ETH_CH390_ENABLE is off or pins are unset (-1).
 * Call after wifi_management_foundation_init() (needs esp_netif + default loop).
 */
esp_err_t board_ethernet_ch390_init(void);

/** CH390 驱动已安装（netif 已创建）。 */
bool board_ethernet_ch390_is_ready(void);

/** esp_eth 链路任务是否在运行。 */
bool board_ethernet_ch390_is_started(void);

/** True when PHY reports cable / link up. */
bool board_ethernet_ch390_link_up(void);

/** Copy IPv4 string for eth netif; returns false if no IP. */
bool board_ethernet_ch390_get_ip(char *buf, size_t buf_len);

/**
 * PHY 链路信息。@p mbps 为 10 或 100；无链路时返回 false。
 */
bool board_ethernet_ch390_get_link_info(int *mbps, bool *full_duplex, bool *link_up);

/** Pause CH390 SPI xfers (e.g. RS485 测试期间)，避免低堆 DMA 分配崩溃。 */
void board_ethernet_ch390_set_traffic_paused(bool paused);

/** 堆恢复或 Tab/全测打开以太网时尝试 esp_eth_start。 */
esp_err_t board_ethernet_ch390_try_start(void);

/** WiFi/BLE 启动后暂停 CH390（测试台 deferred start）。 */
void board_ethernet_ch390_suspend_for_coexist(void);

#ifdef __cplusplus
}
#endif
