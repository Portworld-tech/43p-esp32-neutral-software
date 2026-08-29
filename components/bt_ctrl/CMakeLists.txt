# Customer SDK - prebuilt closed component (bt_ctrl). Implementation sources are not shipped.
idf_component_register(
    INCLUDE_DIRS "include"
    REQUIRES
        bt
        nvs_flash
        esp_event
        freertos
)
set(_CLOSED_LIB "${CMAKE_CURRENT_LIST_DIR}/lib/${IDF_TARGET}/libbt_ctrl.a")
if(NOT EXISTS "${_CLOSED_LIB}")
    message(FATAL_ERROR "bt_ctrl: missing prebuilt ${_CLOSED_LIB}")
endif()
add_prebuilt_library(bt_ctrl_prebuilt "${_CLOSED_LIB}"
    REQUIRES
        bt
        nvs_flash
        esp_event
        freertos
)
target_link_libraries(${COMPONENT_LIB} INTERFACE bt_ctrl_prebuilt)