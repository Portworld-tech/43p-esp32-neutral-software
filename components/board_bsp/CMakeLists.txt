# Customer SDK - prebuilt closed component (board_bsp). Implementation sources are not shipped.
idf_component_register(
    INCLUDE_DIRS "include"
    REQUIRES
        esp_driver_i2c
        esp_driver_gpio
        esp_driver_spi
        esp_driver_ledc
        esp_eth
        esp_netif
        esp_event
        espressif__esp_io_expander
        espressif__esp_io_expander_tca95xx_16bit
        espressif__esp_lvgl_port
        espressif__esp_lcd_panel_io_additions
        espressif__esp_lcd_st7701
        espressif__esp_lcd_touch_gt911
        espressif__ch390
        lvgl__lvgl
        heap
        esp_mm
)
set(_CLOSED_LIB "${CMAKE_CURRENT_LIST_DIR}/lib/${IDF_TARGET}/libboard_bsp.a")
if(NOT EXISTS "${_CLOSED_LIB}")
    message(FATAL_ERROR "board_bsp: missing prebuilt ${_CLOSED_LIB}")
endif()
add_prebuilt_library(board_bsp_prebuilt "${_CLOSED_LIB}"
    REQUIRES
        esp_driver_i2c
        esp_driver_gpio
        esp_driver_spi
        esp_driver_ledc
        esp_eth
        esp_netif
        esp_event
        espressif__esp_io_expander
        espressif__esp_io_expander_tca95xx_16bit
        espressif__esp_lvgl_port
        espressif__esp_lcd_panel_io_additions
        espressif__esp_lcd_st7701
        espressif__esp_lcd_touch_gt911
        espressif__ch390
        lvgl__lvgl
        heap
        esp_mm
)
target_link_libraries(${COMPONENT_LIB} INTERFACE board_bsp_prebuilt)