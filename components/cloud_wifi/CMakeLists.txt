# Customer SDK - prebuilt closed component (cloud_wifi). Implementation sources are not shipped.
idf_component_register(
    INCLUDE_DIRS "include"
    REQUIRES
        esp_wifi
        esp_coex
        esp_netif
        esp_event
        nvs_flash
        mqtt
        freertos
        lvgl__lvgl
        gui_task
        bt_ctrl
)
set(_CLOSED_LIB "${CMAKE_CURRENT_LIST_DIR}/lib/${IDF_TARGET}/libcloud_wifi.a")
if(NOT EXISTS "${_CLOSED_LIB}")
    message(FATAL_ERROR "cloud_wifi: missing prebuilt ${_CLOSED_LIB}")
endif()
add_prebuilt_library(cloud_wifi_prebuilt "${_CLOSED_LIB}"
    REQUIRES
        esp_wifi
        esp_coex
        esp_netif
        esp_event
        nvs_flash
        mqtt
        freertos
        lvgl__lvgl
        gui_task
        bt_ctrl
)
target_link_libraries(${COMPONENT_LIB} INTERFACE cloud_wifi_prebuilt)
