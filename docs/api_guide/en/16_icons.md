# 16. Icons

Replace PNGs under `spiffs_image/icons/nt/*.png`, then `idf.py flash` (SPIFFS storage must be flashed).

APIs: `app_spiffs_mount`, `hub_ico_cache_init`, `hub_ico_add`.

Blank icons usually mean storage was not flashed, or POSIX FS / PNG path mismatch — see [FAQ](./20_faq.md).

---

[← Hub model & UI](./15_hub.md) | [README](./README.md) | [Health monitor →](./17_health.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
