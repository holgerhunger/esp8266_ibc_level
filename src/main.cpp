// IBC Level Sensor
// Autor: Holger Hunger
// 17.05.2026

#include <Arduino.h>
#include <Wire.h>
#include "globals.h"
#include "wifimqtt.h"
#include "display.h"

IbcDisplay display;

void setup() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LED_OFF);

    Wire.begin(SDA, SCL);
    display.begin();
}

void loop() {
    int dummyPercent = 45;
    bool dummyWifi = true;

    display.show(dummyPercent, dummyWifi);

    delay(60000);
}
