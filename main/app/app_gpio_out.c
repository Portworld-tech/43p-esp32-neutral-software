#include "app_gpio_out.h"

#include "board_io_expander.h"
#include "esp_log.h"

static const char *TAG = "app_gpio_out";

/* NCA9555 P1.0 — same mask as board_bsp GPIO_OUT (do not expose full pin map). */
#define APP_GPIO_OUT_PIN_MASK ((esp_io_expander_pin_num_t)(1ULL << 8))

esp_err_t app_gpio_out_set(bool high)
{
    esp_err_t err = board_io_expander_gpio_out_set(high);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "gpio_out_set(%d): %s", (int)high, esp_err_to_name(err));
    }
    return err;
}

esp_err_t app_gpio_out_get(bool *high_out)
{
    if (high_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_io_expander_handle_t exp = board_io_expander_get();
    if (exp == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint32_t level = 0;
    esp_err_t err = esp_io_expander_get_level(exp, (esp_io_expander_pin_num_t)APP_GPIO_OUT_PIN_MASK, &level);
    if (err != ESP_OK) {
        return err;
    }
    *high_out = (level & APP_GPIO_OUT_PIN_MASK) != 0;
    return ESP_OK;
}
