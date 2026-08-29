# cmake/ — 共享构建脚本

## lvgl_psram_heap.cmake

强制链接 `lv_mem_psram_malloc`（Withthewind PSRAM 分配器）。在 `project()` 之后调用：

```cmake
include("${FRAME_MONOREPO_ROOT}/cmake/lvgl_psram_heap.cmake")
lvgl_psram_heap_force_link()
```

## esp_lvgl_port 补丁

主工程 `managed_components/espressif__esp_lvgl_port` 含以下本地修改（相对官方 2.7.2）：

- `lvgl_port_disp_wait_rgb_trans_done()` — `board_display_present_fb()` 使用
- `lvgl8/esp_lvgl_port_disp.c` — `trans_sem` 空指针检查，避免 `fbs=1` 时断言

`esp32_s3_frame` 通过 `main/idf_component.yml` 的 `override_path` 指向同一目录。修改补丁后两工程均需 `idf.py reconfigure build`。
