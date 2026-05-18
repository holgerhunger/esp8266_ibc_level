// IBC Level Sensor
// Autor: Holger Hunger
// 17.05.2026

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "globals.h"
#include "wifimqtt.h"

// SSD1306 Controller adressiert intern 128x64; sichtbar sind 64x48 (oben links)
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void showDisplay(int percent, bool wifiOk) {
    display.clearDisplay();

    // Sichtbarer Bereich beginnt bei Buffer-Spalte 32 (Panel an SEG32-SEG95)
    // WLAN-Status oben links
    display.setTextSize(1);
    display.setCursor(32, 16);
    display.print(wifiOk ? "WiFi" : "----");

    // Füllstand groß
    display.setTextSize(2);
    display.setCursor(40, 40);
    display.print(percent);
    display.print("%");

    display.display();
}

void setup() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LED_OFF); // simuliert: WLAN vorhanden

    Wire.begin(SDA, SCL);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(SSD1306_WHITE);
}

void loop() {
    int dummyPercent = 45;
    bool dummyWifi = true;

    showDisplay(dummyPercent, dummyWifi);

    delay(60000); // jede Minute aktualisieren
}
