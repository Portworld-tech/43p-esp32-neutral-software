if(NOT DEFINED UI_DIR)
    message(FATAL_ERROR "UI_DIR is required")
endif()

set(UI_HELPERS_H "${UI_DIR}/ui_helpers.h")
set(UI_HELPERS_C "${UI_DIR}/ui_helpers.c")

if(EXISTS "${UI_HELPERS_H}")
    file(READ "${UI_HELPERS_H}" H_CONTENT)
    string(REPLACE
        "void _ui_image_set_property(lv_obj_t * target, int id, uint8_t * val);"
        "void _ui_image_set_property(lv_obj_t * target, int id, const void * val);"
        H_CONTENT
        "${H_CONTENT}")
    file(WRITE "${UI_HELPERS_H}" "${H_CONTENT}")
endif()

if(EXISTS "${UI_HELPERS_C}")
    file(READ "${UI_HELPERS_C}" C_CONTENT)
    string(REPLACE
        "void _ui_image_set_property(lv_obj_t * target, int id, uint8_t * val)"
        "void _ui_image_set_property(lv_obj_t * target, int id, const void * val)"
        C_CONTENT
        "${C_CONTENT}")
    file(WRITE "${UI_HELPERS_C}" "${C_CONTENT}")
endif()
