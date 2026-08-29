get_filename_component(APP_HEALTH_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../main/app" ABSOLUTE)

idf_component_register(
    SRCS "${APP_HEALTH_ROOT}/app_health.c"
    INCLUDE_DIRS "${APP_HEALTH_ROOT}"
    REQUIRES esp_timer
)
