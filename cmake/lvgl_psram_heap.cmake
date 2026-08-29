# Force-link lv_mem_psram_malloc so LV_MEM_CUSTOM allocator in withthewind_board_lvgl_init
# is always pulled from the archive (GNU ld member selection order).
#
# Call after project() with no arguments — uses CMAKE_PROJECT_NAME.elf.
macro(lvgl_psram_heap_force_link)
    set(_lvgl_psram_elf "${CMAKE_PROJECT_NAME}.elf")
    if(NOT _lvgl_psram_elf OR _lvgl_psram_elf STREQUAL ".elf")
        message(FATAL_ERROR "lvgl_psram_heap_force_link: call after project() (CMAKE_PROJECT_NAME is empty)")
    endif()
    cmake_language(DEFER CALL target_link_options "${_lvgl_psram_elf}" PRIVATE
        "-Wl,-u,lv_mem_psram_malloc")
endmacro()
