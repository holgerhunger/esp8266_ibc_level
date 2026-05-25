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
    sensor.begin();
    ldr.begin();

    connectAP();
    client.setServer(MQTT_HOST, MQTT_PORT);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    int raw = sensor.readCm();
    int level = (raw > 0) ? constrain(TANK_SENSOR_HEIGHT_CM - raw, 0, TANK_SENSOR_HEIGHT_CM) : -1;
    int pct = (level >= 0) ? constrain(level * 100 / TANK_OVERFLOW_CM, 0, 100) : -1;
    int adc = ldr.readRaw();
    bool wifiOk = (WiFi.status() == WL_CONNECTED);
    bool displayOn = (adc <= LDR_DARK_THRESHOLD);

    display.setPower(displayOn);
    if (displayOn) {
        display.show(pct, wifiOk);
    }
    if (level >= 0) {
        publish(level, pct, adc);
    }

    delay(2000);
}
