# Customer SDK - prebuilt closed component (hub_core). Implementation sources are not shipped.
idf_component_register(
    INCLUDE_DIRS "include"
    REQUIRES
        nvs_flash
)
set(_CLOSED_LIB "${CMAKE_CURRENT_LIST_DIR}/lib/${IDF_TARGET}/libhub_core.a")
if(NOT EXISTS "${_CLOSED_LIB}")
    message(FATAL_ERROR "hub_core: missing prebuilt ${_CLOSED_LIB}")
endif()
add_prebuilt_library(hub_core_prebuilt "${_CLOSED_LIB}"
    REQUIRES
        nvs_flash
)
target_link_libraries(${COMPONENT_LIB} INTERFACE hub_core_prebuilt)