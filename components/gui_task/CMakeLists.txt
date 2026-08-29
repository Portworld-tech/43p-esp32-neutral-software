get_filename_component(GUI_TASK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../main/gui" ABSOLUTE)
idf_component_register(
    SRCS
        "${GUI_TASK_ROOT}/gui_task.c"
        "${GUI_TASK_ROOT}/lvgl_rt_tuning.c"
    INCLUDE_DIRS "${GUI_TASK_ROOT}"
    REQUIRES espressif__esp_lvgl_port lvgl__lvgl esp_driver_gptimer
)
