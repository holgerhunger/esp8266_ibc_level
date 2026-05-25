# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

IoT firmware for an ESP8266 D1 Mini that monitors fill levels of IBC (Intermediate Bulk Container) tanks using an HC-SR04 ultrasonic distance sensor. Ambient brightness is measured via an LDR5528 on the ADC input. Readings are published via MQTT as JSON and shown on an SSD1306 OLED display.

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

- **`main.cpp`** — `setup()` / `loop()` entry point. Reads sensor and LDR, publishes JSON via MQTT, updates display every 2 s.
- **`globals.h`** — Pin and timing constants: `LED=2` (active LOW), I2C `SDA=4`/`SCL=5`, HC-SR04 `TRIGGER_PIN=14`/`ECHO_PIN=13`, `MQTT_TOPIC="keller/ibc_level"`, `WLAN_CONNECT_WAIT=1000`.
- **`myConfig.h`** — WiFi and MQTT credentials (`WLAN_SSID`, `WLAN_PASSWORD`, `MQTT_HOST`, `MQTT_PORT`, `MQTT_USER`, `MQTT_PASSWORD`). **Gitignored** — must be created manually on each dev machine.
- **`wifimqtt.h`** — WiFi/MQTT logic as inline functions: `connectAP()`, `reconnect()`, `callback()`. The MQTT `callback()` handles `test`, `ledState`, and `testJSON` topics.
- **`sensor.h`** — `IbcSensor` class: HC-SR04 ultrasonic distance measurement, 5-sample average, returns cm or -1 on error.
- **`brightness.h`** — `LdrSensor` class: LDR5528 on A0, 4-sample average, returns raw ADC value 0–1023 (0=hell, 1023=dunkel).
- **`display.h`** — `IbcDisplay` class: SSD1306 128×64 OLED, shows WiFi status and distance in cm.

## Current State

Fully operational. MQTT JSON format: `{"cm": 123, "adc": 512}`.

Open items:
- Fill level calculation (sensor distance → 0–100%, tank height ~90 cm, sensor ~5 cm above top)
- LDR-Kalibrierung: Grenzwerte für hell/dunkel noch nicht definiert

## Hardware

| Component | Detail |
|-----------|--------|
| MCU | AZ-Delivery ESP8266 D1 Mini |
| Distance sensor | HC-SR04 (Trigger=GPIO14, Echo=GPIO13) |
| Brightness sensor | LDR5528 on A0; divider: 3.3V — 100k — A0 — LDR — GND |
| Display | 0.66" SSD1306 OLED D1 Mini shield (same I2C bus) |
| Onboard LED | GPIO2, active LOW |
| MQTT broker | Public IP, port 1883 (credentials in `myConfig.h`) |

## Key Libraries (platformio.ini)

Active: `knolleary/PubSubClient@^2.8.0`, `bblanchon/ArduinoJson@^7.1.0`, `ESP8266WiFi`, `adafruit/Adafruit SSD1306`, `adafruit/Adafruit GFX Library`