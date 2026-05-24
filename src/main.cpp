// IBC Level Sensor
// Autor: Holger Hunger
// 17.05.2026

#include <Arduino.h>
#include <Wire.h>
#include "globals.h"
#include "wifimqtt.h"
#include "display.h"
#include "sensor.h"

IbcDisplay display;
IbcSensor sensor;

void publish(int cm) {
    JsonDocument doc;
    doc["cm"] = cm;
    char buf[32];
    serializeJson(doc, buf);
    client.publish(MQTT_TOPIC, buf);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LED_OFF);

    Wire.begin(SDA, SCL);
    display.begin();
    sensor.begin();

    connectAP();
    client.setServer(MQTT_HOST, MQTT_PORT);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    int cm = sensor.readCm();
    bool wifiOk = (WiFi.status() == WL_CONNECTED);

    display.show(cm, wifiOk);
    if (cm > 0) {
        publish(cm);
    }

    delay(2000);
}
