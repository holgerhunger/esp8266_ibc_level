# Projektübersicht

IoT-Entfernungssensor zum Überwachen des Füllstandes von 3× 1000-Liter-IBC-Containern.
Die drei Container sind untereinander verbunden, daher wird nur ein Sensor an einem Container benötigt.
Ausgabe auf Display und Datenübertragung über WLAN an einen MQTT-Server.

Die Füllhöhe im Container beträgt ca. 90 cm. Der Sensor wird ca. 5 cm oberhalb des Containerrandes angebracht.
Berechnet wird der Füllstand in Prozent (0–100 %) und Zentimeter. Liter-Berechnung ist für später vorgesehen.

# Techstack

- **IDE:** JetBrains CLion mit PlatformIO-Plugin
- **Framework:** Arduino (ESP8266 Arduino Core)
- **Sprache:** C++

Datenblätter befinden sich im Ordner `data-sheets/`.  
Zugangsdaten für WLAN und MQTT-Server: `src/myConfig.h` (nicht im Git).  
Pin-Belegungen und MQTT-Adresse: `src/globals.h`.

# Einzelne Programm-Abschnitte

## A. Mikrocontroller

**Hardware:** AZ-Delivery ESP8266 D1 Mini

## B. LED (Onboard, GPIO2, active LOW)

- Leuchtet dauerhaft, solange keine WLAN-Verbindung besteht.
- Blinkt kurz beim Auslesen des Sensors und beim Senden einer MQTT-Nachricht.
- Zukünftig ggf. auch externe LED.

## C. Display

**Hardware:** 0,66"-OLED-Display mit SSD1306-Treiber-IC (D1-Mini-Shield-Format, I2C)

**Anzeige:**

- Groß: Füllstand nur in %
- Klein (oben links): WLAN-Status als Symbol oder Text

**Aktualisierung:** jede Minute (entspricht dem Messintervall)

## D. Sensor

**Hardware:** VL53L0X Laser-Distanz-Sensormodul (I2C, SDA=GPIO4, SCL=GPIO5)

**Messprinzip:** Der Sensor misst den Abstand von der Sensorunterkante zur Wasseroberfläche.
Da die drei IBCs kommunizierend verbunden sind, ist der Füllstand in allen gleich.

**Berechnung:**

```
Füllhöhe_cm  = 90 − (Messwert_cm − 5)   // = 95 − Messwert_cm
Füllstand_%  = Füllhöhe_cm / 90 × 100
```

Beispiele:

| Messwert | Füllhöhe | Füllstand |
|----------|----------|-----------|
| 5 cm     | 90 cm    | 100 %     |
| 50 cm    | 45 cm    | 50 %      |
| 95 cm    | 0 cm     | 0 %       |

**Messintervall:** alle 60 Sekunden

## E. WLAN

- Verbindung mit hinterlegtem SSID/Passwort (`myConfig.h`).
- LED leuchtet dauerhaft während kein WLAN besteht.
- Bei Verbindungsabbruch: automatischer Wiederverbindungsversuch.
- Nach 15 Fehlversuchen: ESP-Neustart.
- Bei WLAN-Ausfall werden Messwerte verworfen (kein lokales Puffern).

## F. MQTT

**Broker:** Externer Server (Zugangsdaten in `myConfig.h`)  
**Topic:** `keller/ibc_level`  
**Sendezyklus:** alle 5 Minuten  
**QoS:** 0 (Fire and forget)

**Datenformat:** JSON als String

```json
{
  "cm": 45,
  "percent": 50
}
```

*(Liter-Feld wird in einer späteren Version ergänzt)*