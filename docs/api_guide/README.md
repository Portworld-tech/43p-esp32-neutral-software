# Customer SDK API Guide

Product secondary-development APIs only (open `main/app` + closed public headers).

| Lang | TOC |
|------|-----|
| 中文 | [zh/README.md](zh/README.md) |
| English | [en/README.md](en/README.md) |

Both language trees are **hand-maintained**. `_gen_chapters.py` skips every stem listed in `HAND_WRITTEN` (do not use it to “translate” EN).

## Word export

Formatted manuals (cover + TOC field + chapters):

| File | Path |
|------|------|
| 中文 | [export/Customer_SDK_API_Guide_ZH.docx](export/Customer_SDK_API_Guide_ZH.docx) |
| English | [export/Customer_SDK_API_Guide_EN.docx](export/Customer_SDK_API_Guide_EN.docx) |

Regenerate:

```powershell
python docs/api_guide/_export_docx.py
```

Open in Word → right-click the TOC → **Update field** (更新域) to fill page numbers.
