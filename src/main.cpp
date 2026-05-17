// mqtt IBC Level Sensor
// Autor: Holger Hunger
// 17.05.2026

#include <Arduino.h>
#include "globals.h"
#include "wifimqtt.h"

void setup() {
    Serial.begin(115200);

    pinMode(LED, OUTPUT);
    digitalWrite(LED, LED_ON);
}

void loop() {
    delay(500);
    digitalWrite(LED, LED_OFF);
    delay(1500);
    digitalWrite(LED, LED_ON);
    Serial.println("LED Blink!");
}
