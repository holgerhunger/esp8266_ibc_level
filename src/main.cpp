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

void setup() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LED_OFF);

    Wire.begin(SDA, SCL);
    display.begin();
    sensor.begin();
}

void loop() {
    int cm = sensor.readCm();
    int percent = (cm > 0) ? constrain((95 - cm) * 100 / 90, 0, 100) : 0;
    bool dummyWifi = true;

    display.show(percent, dummyWifi);

    delay(5000);
}
