# Word 导出说明 / Word export

| 文件 | 说明 |
|------|------|
| `Customer_SDK_API_Guide_ZH.docx` | 中文完整手册 |
| `Customer_SDK_API_Guide_EN.docx` | English full guide |

## 打开后请先做

1. 用 Microsoft Word 打开  
2. 目录页中 **右键目录域 → 更新域 / Update field**（生成页码与条目）  
3. 如提示更新整个目录，选择「更新整个目录」

## 重新生成

```powershell
cd customer_sdk
python docs/api_guide/_export_docx.py
```

排版约定见 `docs/api_guide/_export_docx.py` 文件头注释。
