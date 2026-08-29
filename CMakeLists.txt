cmake_minimum_required(VERSION 3.19)

# Customer SDK 鈥?no vendor pin profile in SDKCONFIG_DEFAULTS.
set(SDKCONFIG_DEFAULTS
    "${CMAKE_CURRENT_LIST_DIR}/config/sdkconfig.common.defaults"
    "${CMAKE_CURRENT_LIST_DIR}/config/sdkconfig.lvgl.defaults"
    "${CMAKE_CURRENT_LIST_DIR}/sdkconfig.defaults"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
add_compile_options(-Wno-format)
project(lvgl_customer_sdk)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/main/app_ui_theme_select.h")

include("${CMAKE_CURRENT_LIST_DIR}/cmake/lvgl_psram_heap.cmake")
lvgl_psram_heap_force_link()

spiffs_create_partition_image(storage spiffs_image FLASH_IN_PROJECT)
