// Project-level Bluetooth management facade (BR/EDR + BLE + BLE Mesh)
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#include "bt_gatt_star.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BT_TRANSPORT_NONE = 0,
    BT_TRANSPORT_BLE_GATT,
    BT_TRANSPORT_BREDR_SPP,
    BT_TRANSPORT_MESH_PROXY,
    BT_TRANSPORT_MESH_VENDOR,
} bt_transport_t;

typedef enum {
    BT_AUTH_NONE = 0,
    BT_AUTH_BONDED,
    BT_AUTH_APP_VERIFIED,
} bt_auth_state_t;

typedef struct {
    bt_transport_t transport;
    bool connected;
    bool encrypted;
    bool bonded;
    bt_auth_state_t auth;
    int8_t rssi; /* 127 when unavailable */
    char peer_name[32];
    char peer_addr[18]; /* "AA:BB:CC:DD:EE:FF" */
} bt_link_state_t;

typedef struct {
    bool enabled_ble;
    bool enabled_bredr;
    bool enabled_mesh;
    bool enable_pairing_ui;
    bool single_controller_lock;
    uint16_t ble_mtu;
    const char *device_name; /* NULL -> default */
} bt_mgmt_config_t;

/**
 * @brief Initialize bt_management subsystem (no RF yet).
 *
 * This assumes NVS + default event loop have been created already.
 */
esp_err_t bt_management_init(const bt_mgmt_config_t *cfg);

/**
 * @brief Start BT controller/host and configured profiles.
 */
esp_err_t bt_management_start(void);

/**
 * @brief Stop BT profiles and deinit stack if possible.
 */
esp_err_t bt_management_stop(void);

/**
 * @brief Submit a control frame locally (e.g. from UI).
 */
esp_err_t bt_management_local_submit_command(const uint8_t *buf, size_t len);

/**
 * @brief Enable/disable Mesh provisioning (if mesh enabled).
 */
esp_err_t bt_management_mesh_set_prov_enable(bool enable);

/**
 * @brief Whether Mesh provisioning is requested on (matches app-side mesh toggle).
 */
bool bt_management_mesh_prov_is_enabled(void);

/** Snapshot of local BLE Mesh node state (for UI / diagnostics). */
typedef struct {
    bool provisioned;
    bool prov_adv_enabled;
    uint16_t net_idx;
    uint16_t unicast_addr;
    uint16_t app_idx;
    bool send_ready;
    char device_uuid[33]; /*!< 16-byte UUID as hex (for provisioner identification) */
} bt_mesh_status_t;

/**
 * @brief Read local mesh node status (provisioned address, app key readiness, etc.).
 */
void bt_management_mesh_get_status(bt_mesh_status_t *out);

/**
 * @brief Send vendor SET_STATE to another mesh node (unicast or group address).
 *
 * Payload is [item_id][value]. Destination must be on the same mesh network with
 * vendor server model (CID 0x02E5, model 0x0001) bound to an app key.
 */
esp_err_t bt_management_mesh_send_set_state(uint16_t dst_addr, uint8_t item_id, uint8_t value);

/**
 * @brief Re-start BLE GATT connectable advertising (coexists with Mesh when enabled).
 */
void bt_management_ble_refresh_advertising(void);

/** GATT 服务已启动后由 bt_transport_ble 调用，用于延后 Mesh 初始化避免 BTC_TASK 栈溢出 */
void bt_management_on_ble_gatt_ready(void);

/**
 * @brief Factory reset BT settings (bond + app auth + mesh state).
 */
esp_err_t bt_management_factory_reset(void);

/**
 * @brief Get current enabled capabilities bitmask (for HELLO_RSP, etc.).
 *
 * Bit 0: BLE
 * Bit 1: Classic BR/EDR (always 0 on ESP32-S3)
 * Bit 2: BLE Mesh
 */
uint32_t bt_management_caps_mask(void);

/**
 * @brief Apply SET_STATE command to real device.
 *
 * Default implementation only logs and returns ESP_OK.
 * You can override by providing a non-weak implementation elsewhere in the project.
 *
 * @param item_id Logical item ID (v1: 1..255)
 * @param value   Value (v1: 0/1 for off/on)
 */
esp_err_t bt_management_apply_set_state(uint8_t item_id, uint8_t value);

/**
 * @brief NVS 中保存的 Mesh 测试台 UI 默认值（对端地址 / Item ID），供 GATT 查询与小程序同步。
 * 默认 weak 实现返回 0x0002 / 1；ui_shell 可覆盖。
 */
void bt_management_testbench_get_mesh_ui(uint16_t *peer_addr, uint8_t *item_id);

/** GATT 星型链路快照（与 bt_gatt_star_get_link 相同） */
void bt_management_gatt_get_link(bt_gatt_star_link_t *out);

/** 测试台：向已连接 Central 发送 DATA 帧（需已开启 Notify） */
esp_err_t bt_management_gatt_send_data(const uint8_t *data, size_t len);

unsigned bt_management_gatt_log_count(void);
void bt_management_gatt_format_log(char *buf, size_t buf_len, unsigned max_lines);
void bt_management_gatt_log_clear(void);

/** BLE 被动扫描（测试台用），duration_sec 秒后自动停止并恢复广播。 */
esp_err_t bt_management_ble_scan_start(uint32_t duration_sec);
bool bt_management_ble_scan_done(void);
bool bt_management_ble_scan_failed(void);
int bt_management_ble_scan_count(void);
/** 返回最强信号设备摘要，无设备时 buf 为 "(无)"。 */
void bt_management_ble_scan_summary(char *buf, size_t buf_len);

/** 若扫描进行中则停止（以太网测试前释放控制器）。 */
void bt_management_ble_scan_abort_if_active(void);

#ifdef __cplusplus
}
#endif

