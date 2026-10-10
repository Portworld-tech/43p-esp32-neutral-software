# -*- coding: utf-8 -*-
"""Pin esp_lvgl_port -> lvgl to 8.4.* so IDF 5.5 component-manager does not
evaluate LVGL9 optional deps (libjpeg/libpng/lz4) and fail configure.

Safe to re-run; no-op if already pinned or component not downloaded yet.
"""
from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
YML = ROOT / "managed_components" / "espressif__esp_lvgl_port" / "idf_component.yml"

OLD = "version: '>=8,<10'"
NEW = "version: '8.4.*'"


def main() -> int:
    if not YML.is_file():
        print(f"skip: {YML} not present yet")
        return 0
    text = YML.read_text(encoding="utf-8")
    if NEW in text and OLD not in text:
        print(f"ok: already pinned in {YML}")
        return 0
    if OLD not in text:
        print(f"skip: unexpected content in {YML}")
        return 0
    YML.write_text(text.replace(OLD, NEW, 1), encoding="utf-8")
    print(f"patched: {YML} ({OLD} -> {NEW})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
