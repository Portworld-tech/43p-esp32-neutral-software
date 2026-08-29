#pragma once

#include <stddef.h>

void *lv_mem_psram_malloc(size_t size);
void lv_mem_psram_free(void *ptr);
void *lv_mem_psram_realloc(void *ptr, size_t new_size);
