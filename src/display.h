#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class IbcDisplay {
public:
    void begin() {
        _display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
        _display.setTextColor(SSD1306_WHITE);
    }

    void show(int percent, bool wifiOk) {
        _display.clearDisplay();

        // Sichtbarer Bereich beginnt bei Buffer-Spalte 32 (Panel an SEG32-SEG95)
        _display.setTextSize(1);
        _display.setCursor(32, 16);
        _display.print(wifiOk ? "WiFi" : "----");

        _display.setTextSize(2);
        _display.setCursor(40, 40);
        _display.print(percent);
        _display.print("%");

        _display.display();
    }

private:
    // SSD1306 adressiert intern 128x64; sichtbar sind 64x48 (oben links)
    Adafruit_SSD1306 _display{128, 64, &Wire, -1};
};

#endif // DISPLAY_H
