# Sand theme pack

| File | Role |
|------|------|
| `palette.c` | 色板 |
| `theme_local.h/.c` | 本主题回调、chrome、控件辅助 |
| `home.c` | 首页 IA |
| `pages_room.c` | 房间控件 |
| `pages_scenes.c` | 情景 |
| `pages_ops.c` | 能耗 / 总线 / 网络 / 点表 |
| `pages_life.c` | 设置 / 安防 / 日程 / 温控 |
| `boot.c` | `hub_theme_build` + `app_ui_start` |

主题之间不互相引用；只依赖 `main/hub_ui/` 薄运行时。
选型：`APP_UI_THEME_SAND`
