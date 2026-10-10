# 2. Environment & build

- ESP-IDF **5.5.x** · target `esp32s3`
- `idf.py build` / `flash` **must include the SPIFFS storage partition**

```powershell
cd customer_sdk
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

If RS485 uses UART0: switch the console to **USB Serial/JTAG** (matches validated test setup), or override pins via `app_rs485_config_t`.

---

[← Overview](./01_overview.md) | [README](./README.md) | [Boot order →](./03_boot.md)

> Authoritative definitions live in `include/*.h` / `main/app/*.h`. Do not reverse-engineer closed `.a` libraries.
