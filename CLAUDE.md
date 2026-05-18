# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

IoT firmware for an ESP8266 D1 Mini that monitors fill levels of IBC (Intermediate Bulk Container) tanks using a VL53L0X laser ToF distance sensor. Readings are published via MQTT as JSON and shown on an SSD1306 OLED display.

## Build System

This project uses **PlatformIO**. Target board: `d1_mini` (AZ-Delivery ESP8266 D1 Mini).

```bash
pio run                                    # compile only
pio run --target upload                    # compile and flash (921600 baud)
pio device monitor                         # serial monitor at 115200 baud
pio run --target upload && pio device monitor  # flash then monitor
```

No test framework is configured; `test/` directory is empty.

## Architecture

All source lives in `src/`:

- **`main.cpp`** — `setup()` / `loop()` entry point. Currently a LED blink skeleton; WiFi/MQTT/sensor code must be wired in here.
- **`globals.h`** — Pin and timing constants: `LED=2` (active LOW, use `LED_ON=LOW`), I2C `SDA=4`/`SCL=5`, `MQTT_TOPIC="keller/ibc_level"`, `WLAN_CONNECT_WAIT=1000`.
- **`myConfig.h`** — WiFi and MQTT credentials (`WLAN_SSID`, `WLAN_PASSWORD`, `MQTT_HOST`, `MQTT_PORT`, `MQTT_USER`, `MQTT_PASSWORD`). **Gitignored** — must be created manually on each dev machine.
- **`wifimqtt.h`** — WiFi/MQTT logic as inline functions: `connectAP()`, `reconnect()`, `callback()`. These are defined but **not yet called** from `main.cpp`. The MQTT `callback()` handles `test`, `ledState`, and `testJSON` topics.

## Current State (as of first commit)

The LED blink proof-of-life test compiles and runs. The following is not yet implemented:

- VL53L0X distance sensor (library not yet in `lib_deps`, no read code)
- SSD1306 OLED display (library not yet in `lib_deps`, no display code)
- `connectAP()` and `reconnect()` not yet called from `main.cpp`
- MQTT publish of sensor data (target format: JSON `{"cm": 123}`)
- Fill level calculation (sensor distance → 0–100%, tank height ~90 cm, sensor ~5 cm above top)

## Hardware

| Component | Detail |
|-----------|--------|
| MCU | AZ-Delivery ESP8266 D1 Mini |
| Distance sensor | VL53L0X (I2C, SDA=GPIO4, SCL=GPIO5) |
| Display | 0.66" SSD1306 OLED D1 Mini shield (same I2C bus) |
| Onboard LED | GPIO2, active LOW |
| MQTT broker | Public IP, port 1883 (credentials in `myConfig.h`) |

## Key Libraries (platformio.ini)

Active: `knolleary/PubSubClient@^2.8.0`, `bblanchon/ArduinoJson@^7.1.0`, `ESP8266WiFi`

Not yet added (needed next): VL53L0X library, Adafruit SSD1306 / GFX library