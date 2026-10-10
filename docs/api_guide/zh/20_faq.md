# 20. FAQ

| 问题 | 处理 |
|------|------|
| 图标空白 | flash storage；PNG + POSIX FS |
| RS485 无响应 | 查 DE；控制台是否占 UART0；`app_rs485_is_ready` |
| 链接找不到 expander 符号 | 已恢复 `board_io_expander.h` 客户子集；确认链接 `board_bsp.a` |
| 按键无蜂鸣 | `app_beep_click_if_enabled` 或检查 click_sound |

---

[← 控制通路](./19_control_map.md) | [目录](./README.md) | [头文件索引 →](./A_headers.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
