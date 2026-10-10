# API 手册维护说明

## 目录角色

| 路径 | 角色 |
|------|------|
| `zh/00_secondary_dev.md` | 二次开发总入口（手写，勿被生成器覆盖） |
| `zh/guides/G*.md` | 多技术融合场景（手写） |
| `zh/10/11/14/15_*.md` | 总线/UI 详细章（手写加强） |
| 其它 `zh/0x_*.md` | 可由 `_gen_chapters.py` 生成 |
| `main/app/examples/` | 可复制的总线桥接示例 |

## 客户阅读顺序

`00_secondary_dev` → **`AI_DEV` / `AI_CONTEXT`（若用 AI）** → `guides/G00` → `guides/G01` → 技术分章按需

## AI 相关文档（手写）

| 文件 | 用途 |
|------|------|
| `zh/AI_CONTEXT.md` | 置顶约束卡，减少幻觉 |
| `zh/AI_DEV.md` | 工作流、模板提示词、红线审查 |
| `zh/guides/G06_ai_prompts.md` | 短提示词速查 |
| 仓库根 `AGENTS.md` | Cursor Agent 入口指针 |

## 生成器注意

`_gen_chapters.py` **不得** `unlink` 整个 `zh/` 再重建。手写文件列表见脚本内 `HAND_WRITTEN`。
