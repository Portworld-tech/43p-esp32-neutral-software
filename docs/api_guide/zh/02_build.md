# 2. 环境与编译

- ESP-IDF **5.5.x** · 目标 `esp32s3`
- `idf.py build` / `flash` **必须含 SPIFFS storage**

```powershell
cd customer_sdk
idf.py set-target esp32s3
idf.py build
idf.py -p COMx flash monitor
```

RS485 若占用 UART0：将控制台改为 **USB Serial/JTAG**（与测试验证一致），或自定义 `app_rs485_config_t` 换口。

---

[← 概览与分层](./01_overview.md) | [目录](./README.md) | [启动顺序 →](./03_boot.md)

> 权威定义以 `include/*.h` / `main/app/*.h` 为准。封闭库（`.a`）勿反汇编。
