#include "aht20.h"
#include "aht20_calib.h"

#include "sdkconfig.h"

#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "aht20";

static i2c_master_dev_handle_t s_aht20_dev;
static bool s_aht20_ready;

#if CONFIG_AHT20_ENABLE

#define AHT20_CMD_SOFT_RESET 0xBA
#define AHT20_CMD_INIT       0xBE
#define AHT20_CMD_TRIGGER    0xAC

static esp_err_t aht20_write(const uint8_t *data, size_t len)
{
    if (s_aht20_dev == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return i2c_master_transmit(s_aht20_dev, data, len, 100);
}

static esp_err_t aht20_read_bytes(uint8_t *data, size_t len)
{
    if (s_aht20_dev == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return i2c_master_receive(s_aht20_dev, data, len, 100);
}

static esp_err_t aht20_read_status(uint8_t *status)
{
    return aht20_read_bytes(status, 1);
}

static esp_err_t aht20_wait_idle(void)
{
    for (int i = 0; i < 20; i++) {
        uint8_t status = 0;
        esp_err_t err = aht20_read_status(&status);
        if (err != ESP_OK) {
            return err;
        }
        if ((status & 0x80U) == 0U) {
            return ESP_OK;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return ESP_ERR_TIMEOUT;
}

static esp_err_t aht20_calibrate(void)
{
    uint8_t status = 0;
    ESP_RETURN_ON_ERROR(aht20_read_status(&status), TAG, "status read");

    if (status & 0x08U) {
        return ESP_OK;
    }

    const uint8_t init_cmd[] = {AHT20_CMD_INIT, 0x08, 0x00};
    ESP_RETURN_ON_ERROR(aht20_write(init_cmd, sizeof(init_cmd)), TAG, "init cmd");
    vTaskDelay(pdMS_TO_TICKS(10));

    ESP_RETURN_ON_ERROR(aht20_read_status(&status), TAG, "status after init");
    if ((status & 0x08U) == 0U) {
        ESP_LOGE(TAG, "calibration failed, status=0x%02x", status);
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}

static void aht20_apply_customer_calib(float *temp_c, float *rh_pct)
{
#if AHT20_CALIB_ENABLE
    if (temp_c != NULL) {
        *temp_c = (*temp_c) * (float)AHT20_TEMP_SCALE + (float)AHT20_TEMP_OFFSET_C;
    }
    if (rh_pct != NULL) {
        float rh = (*rh_pct) * (float)AHT20_RH_SCALE + (float)AHT20_RH_OFFSET_PCT;
#if AHT20_RH_CLAMP
        if (rh < 0.0f) {
            rh = 0.0f;
        }
        if (rh > 100.0f) {
            rh = 100.0f;
        }
#endif
        *rh_pct = rh;
    }
#else
    (void)temp_c;
    (void)rh_pct;
#endif
}

esp_err_t aht20_init(i2c_master_bus_handle_t bus)
{
    if (bus == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_aht20_ready) {
        return ESP_OK;
    }

    const i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CONFIG_AHT20_I2C_ADDR,
        .scl_speed_hz = CONFIG_AHT20_I2C_HZ,
    };

    esp_err_t err = i2c_master_bus_add_device(bus, &dev_cfg, &s_aht20_dev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "add device failed: %s", esp_err_to_name(err));
        return err;
    }

    err = i2c_master_probe(bus, CONFIG_AHT20_I2C_ADDR, 100);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "probe 0x%02x failed: %s", CONFIG_AHT20_I2C_ADDR, esp_err_to_name(err));
        i2c_master_bus_rm_device(s_aht20_dev);
        s_aht20_dev = NULL;
        return err;
    }

    const uint8_t reset = AHT20_CMD_SOFT_RESET;
    ESP_RETURN_ON_ERROR(aht20_write(&reset, 1), TAG, "soft reset");
    vTaskDelay(pdMS_TO_TICKS(20));

    err = aht20_calibrate();
    if (err != ESP_OK) {
        i2c_master_bus_rm_device(s_aht20_dev);
        s_aht20_dev = NULL;
        return err;
    }

    s_aht20_ready = true;
    ESP_LOGI(TAG, "ready on I2C 0x%02x (%d Hz); calib=%d T=%g*%g R=%g*%g",
             CONFIG_AHT20_I2C_ADDR, CONFIG_AHT20_I2C_HZ,
             (int)AHT20_CALIB_ENABLE,
             (double)AHT20_TEMP_SCALE, (double)AHT20_TEMP_OFFSET_C,
             (double)AHT20_RH_SCALE, (double)AHT20_RH_OFFSET_PCT);
    return ESP_OK;
}

esp_err_t aht20_read_raw(float *temp_c, float *rh_pct)
{
    if (temp_c == NULL && rh_pct == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_aht20_ready || s_aht20_dev == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    const uint8_t trigger[] = {AHT20_CMD_TRIGGER, 0x33, 0x00};
    ESP_RETURN_ON_ERROR(aht20_write(trigger, sizeof(trigger)), TAG, "trigger");
    ESP_RETURN_ON_ERROR(aht20_wait_idle(), TAG, "measure timeout");

    uint8_t data[6] = {0};
    ESP_RETURN_ON_ERROR(aht20_read_bytes(data, sizeof(data)), TAG, "read data");

    /* AHT20: humidity 20-bit in data[1..3], temperature 20-bit in data[3..5]. */
    const uint32_t raw_h = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) |
                           ((uint32_t)data[3] >> 4);
    const uint32_t raw_t = (((uint32_t)data[3] & 0x0FU) << 16) | ((uint32_t)data[4] << 8) |
                           (uint32_t)data[5];

    if (rh_pct) {
        float rh = (float)raw_h * 100.0f / 1048576.0f;
        if (rh < 0.0f) {
            rh = 0.0f;
        }
        if (rh > 100.0f) {
            rh = 100.0f;
        }
        *rh_pct = rh;
    }
    if (temp_c) {
        *temp_c = ((float)raw_t * 200.0f / 1048576.0f) - 50.0f;
    }
    return ESP_OK;
}

esp_err_t aht20_read(float *temp_c, float *rh_pct)
{
    esp_err_t err = aht20_read_raw(temp_c, rh_pct);
    if (err == ESP_OK) {
        aht20_apply_customer_calib(temp_c, rh_pct);
    }
    return err;
}

esp_err_t aht20_read_temperature_c(float *temp_c)
{
    return aht20_read(temp_c, NULL);
}

esp_err_t aht20_read_humidity_rh(float *rh_pct)
{
    return aht20_read(NULL, rh_pct);
}

#else /* !CONFIG_AHT20_ENABLE */

esp_err_t aht20_init(i2c_master_bus_handle_t bus)
{
    (void)bus;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t aht20_read_raw(float *temp_c, float *rh_pct)
{
    (void)temp_c;
    (void)rh_pct;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t aht20_read(float *temp_c, float *rh_pct)
{
    (void)temp_c;
    (void)rh_pct;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t aht20_read_temperature_c(float *temp_c)
{
    (void)temp_c;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t aht20_read_humidity_rh(float *rh_pct)
{
    (void)rh_pct;
    return ESP_ERR_NOT_SUPPORTED;
}

#endif /* CONFIG_AHT20_ENABLE */
