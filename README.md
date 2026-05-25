# Projektübersicht

IoT-Füllstandssensor zum Überwachen von 3× 1000-Liter-IBC-Containern.
Die Container stehen in einem Keller.
Die drei Container sind untereinander verbunden, daher wird nur ein Sensor an einem Container benötigt.
An einem Behälter ist ein Loch mit Leitung zu einem Gulli als Überlauf.
Ausgabe auf Display und Datenübertragung über WLAN an einen MQTT-Server.

Der Sensor wird oberhalb des Containerrandes angebracht und misst den Abstand zur Wasseroberfläche.
Falls der Sensor nicht sehr lange hält, wird er noch durch einen Wasserdichten ersetzt.
Berechnet wird der Füllstand in Zentimeter ab Boden und in Prozent (0–100 %).
Liter-Berechnung ist für später vorgesehen.

# Techstack

- **IDE:** JetBrains CLion mit PlatformIO-Plugin
- **Framework:** Arduino (ESP8266 Arduino Core)
- **Sprache:** C++

Datenblätter befinden sich im Ordner `data-sheets/`.  
Zugangsdaten für WLAN und MQTT-Server: `src/myConfig.h` (nicht im Git).  
Pin-Belegungen, Schwellwerte und Intervalle: `src/globals.h`.

# Einzelne Programm-Abschnitte

## A. Mikrocontroller

**Hardware:** AZ-Delivery ESP8266 D1 Mini

## B. LED (Onboard, GPIO2, active LOW)

- Leuchtet dauerhaft, solange keine WLAN-Verbindung besteht.
- Leuchtet während MQTT-Verbindungsaufbau.
- Zukünftig ggf. auch externe LED.

## C. Display

**Hardware:** AZ-Delivery 0,96"-OLED-Display mit SSD1306-Treiber-IC (128×64 Pixel, I2C: GND/VCC/SCL/SDA)

**Anzeige:**

- Groß: Füllstand in %
- Klein (oben links): WLAN-Status als Text

**Lebensdauerschutz:** OLED-Pixel altern bei Dauerbetrieb. Daher wird das Display über den
LDR-Helligkeitssensor (siehe G) gesteuert: Bei Dunkelheit (ADC > `LDR_DARK_THRESHOLD` = 750)
wird es per SSD1306-Befehl vollständig abgeschaltet (`SSD1306_DISPLAYOFF`). Bei ausreichend
Helligkeit schaltet es sich wieder ein.

**Aktualisierung:** alle 5 Sekunden, solange Display eingeschaltet.

## D. Sensor

**Hardware:** HC-SR04 Ultraschall-Distanzsensor (Trigger=GPIO14, Echo=GPIO13)

**Messprinzip:** Der Sensor misst den Abstand von seiner Unterseite zur Wasseroberfläche.
Da die drei IBCs kommunizierend verbunden sind, ist der Füllstand in allen gleich.
Pro Messung werden 5 Einzelwerte gemittelt.

**Geometrie-Konstanten (`globals.h`):**

| Konstante               | Wert   | Bedeutung                                          |
|-------------------------|--------|----------------------------------------------------|
| `TANK_SENSOR_HEIGHT_CM` | 106 cm | Abstand Behälterboden bis Sensor                   |
| `TANK_OVERFLOW_CM`      | 88 cm  | Füllstand bei dem der Behälter überläuft (= 100 %) |

**Berechnung:**

```
level_cm  = TANK_SENSOR_HEIGHT_CM − Messwert_cm   // Füllstand ab Boden
pct       = level_cm × 100 / TANK_OVERFLOW_CM      // 0–100 %
```

Beispiele:

| Messwert (Sensor) | Füllstand (cm) | Füllstand (%) |
|-------------------|----------------|---------------|
| 18 cm             | 88 cm          | 100 %         |
| 53 cm             | 53 cm          | 60 %          |
| 106 cm            | 0 cm           | 0 %           |

## E. WLAN

- Verbindung mit hinterlegtem SSID/Passwort (`myConfig.h`).
- LED leuchtet dauerhaft während kein WLAN besteht.
- Bei Verbindungsabbruch: automatischer Wiederverbindungsversuch.
- Nach 30 Fehlversuchen beim WLAN-Aufbau bzw. 10 beim MQTT-Aufbau: ESP-Neustart.
- Bei WLAN-Ausfall werden Messwerte verworfen (kein lokales Puffern).

## F. MQTT

**Broker:** Externer Server (Zugangsdaten in `myConfig.h`)  
**Topic:** `keller/ibc_level`  
**Sendezyklus:** alle 5 Minuten (`MQTT_INTERVAL_MS` = 300 000 ms)  
**QoS:** 0 (Fire and forget)

**Datenformat:** JSON als String

```json
{
  "cm": 53,
  "pct": 60,
  "adc": 312
}
```

| Feld  | Bedeutung                                   |
|-------|---------------------------------------------|
| `cm`  | Füllstand in cm ab Behälterboden            |
| `pct` | Füllstand in Prozent (0–100)                |
| `adc` | Rohwert des LDR-Helligkeitssensors (0–1023) |

*(Liter-Feld wird in einer späteren Version ergänzt)*

## G. Helligkeitssensor

**Hardware:** LDR5528 an ADC-Eingang A0  
**Schaltung:** 3,3 V — 100 kΩ — A0 — LDR — GND

Der ADC-Wert steigt bei Dunkelheit (hoher LDR-Widerstand → höhere Spannung an A0).
Pro Messung werden 4 Einzelwerte gemittelt.

| ADC-Wert                   | Bedeutung                       |
|----------------------------|---------------------------------|
| 0                          | sehr hell                       |
| 750 (`LDR_DARK_THRESHOLD`) | Schwellwert Display-Abschaltung |
| 1023                       | sehr dunkel                     |

# Mess- und Publish-Logik

Der Haupt-Loop läuft mit einem festen Takt von 5 Sekunden (`DISPLAY_INTERVAL_MS`).
Messen und Publishen erfolgt abhängig vom Display-Zustand und dem MQTT-Timer:

| Display-Zustand | Zeit seit letztem Publish | Sensor messen | Display aktualisieren | MQTT senden |
|-----------------|---------------------------|---------------|-----------------------|-------------|
| AN              | < 5 Minuten               | ✓             | ✓                     | –           |
| AN              | ≥ 5 Minuten               | ✓             | ✓                     | ✓           |
| AUS             | < 5 Minuten               | –             | –                     | –           |
| AUS             | ≥ 5 Minuten               | ✓             | –                     | ✓           |

Beim Start wird `lastPublish` so initialisiert, dass das erste MQTT-Publish sofort beim
ersten Loop-Durchlauf erfolgt.