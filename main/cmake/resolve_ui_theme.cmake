# Resolve UI_THEME_ID / UI_THEME_IS_HUB from main/app_ui_theme_select.h
# Customer SDK: hub themes only (no SquareLine default).

get_filename_component(_UI_THEME_MAIN_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_UI_THEME_SELECT_H "${_UI_THEME_MAIN_DIR}/app_ui_theme_select.h")

if(NOT EXISTS "${_UI_THEME_SELECT_H}")
    message(FATAL_ERROR "Missing UI theme select header: ${_UI_THEME_SELECT_H}")
endif()

set(UI_THEME_ID "")
file(STRINGS "${_UI_THEME_SELECT_H}" _ui_theme_lines)
foreach(_line IN LISTS _ui_theme_lines)
    if(_line MATCHES "^[ \t]*#define[ \t]+APP_UI_THEME_ID[ \t]+APP_UI_THEME_([A-Za-z0-9_]+)")
        string(TOLOWER "${CMAKE_MATCH_1}" UI_THEME_ID)
        break()
    endif()
endforeach()

if(UI_THEME_ID STREQUAL "")
    message(FATAL_ERROR "APP_UI_THEME_ID not found in app_ui_theme_select.h")
endif()

set(_UI_THEME_VALID slate sand ink forest dusk ocean zen pulse bloom metro)
list(FIND _UI_THEME_VALID "${UI_THEME_ID}" _ui_theme_idx)
if(_ui_theme_idx LESS 0)
    message(FATAL_ERROR
        "Invalid APP_UI_THEME_ID 鈫?'${UI_THEME_ID}'. "
        "Customer SDK hub themes only: ${_UI_THEME_VALID}. "
        "Edit main/app_ui_theme_select.h (do not use APP_UI_THEME_DEFAULT).")
endif()

set(UI_THEME_IS_HUB 1)
set(UI_THEME_MAIN_DIR "${_UI_THEME_MAIN_DIR}")
message(STATUS "UI theme pack (from app_ui_theme_select.h): ${UI_THEME_ID}")
