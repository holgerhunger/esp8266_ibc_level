// IBC Level Sensor
// Autor: Holger Hunger
// 17.05.2026

#include <Arduino.h>
#include <Wire.h>
#include "globals.h"
#include "wifimqtt.h"
#include "display.h"
#include "sensor.h"
#include "brightness.h"

IbcDisplay display;
IbcSensor sensor;
LdrSensor ldr;

unsigned long lastPublish = -MQTT_INTERVAL_MS; // erstes Publish sofort auslösen

void publish(int level, int pct, int adc) {
    JsonDocument doc;
    doc["cm"] = level;
    doc["pct"] = pct;
    doc["adc"] = adc;
    char buf[64];
    serializeJson(doc, buf);
    client.publish(MQTT_TOPIC, buf);
}

void setup() {
    //Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LED_OFF);

    Wire.begin(SDA, SCL);
    display.begin();
    display.show(-1, false);
    sensor.begin();
    ldr.begin();

    connectAP();
    client.setServer(MQTT_HOST, MQTT_PORT);
    client.setCallback(callback);
}

void loop() {
    unsigned long now = millis();
    int adc = ldr.readRaw();
    bool displayOn = (adc <= LDR_DARK_THRESHOLD);
    bool timeToPublish = (now - lastPublish >= MQTT_INTERVAL_MS);

    display.setPower(displayOn);

    if (displayOn || timeToPublish) {
        int raw = sensor.readCm();
        int level = (raw > 0) ? constrain(TANK_SENSOR_HEIGHT_CM - raw, 0, TANK_SENSOR_HEIGHT_CM) : -1;
        int pct = (level >= 0) ? constrain(level * 100 / TANK_OVERFLOW_CM, 0, 100) : -1;

        // Display läuft unabhängig vom WLAN-Status
        if (displayOn) {
            display.show(pct, WiFi.status() == WL_CONNECTED);
        }

        // WLAN/MQTT erst jetzt – kurz vor dem Zugriff – prüfen und ggf. aufbauen
        if (timeToPublish) {
            if (!client.connected()) {
                reconnect();
            }
            if (client.connected()) {
                client.loop();
                if (level >= 0) {
                    publish(level, pct, adc);
                }
            }
            // Intervall (und den Restart-Zähler in reconnect) im 5-Min-Takt halten,
            // auch wenn die Verbindung gerade nicht steht
            lastPublish = now;
        }
    }

    delay(DISPLAY_INTERVAL_MS);
}
