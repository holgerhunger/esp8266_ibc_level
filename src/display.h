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

    void show(int cm, bool wifiOk) {
        _display.clearDisplay();

        _display.setTextSize(1);
        _display.setCursor(0, 0);
        _display.print(wifiOk ? "WiFi OK" : "no WiFi");

        _display.setTextSize(3);
        _display.setCursor(10, 22);
        if (cm < 0) {
            _display.print("---");
        } else {
            _display.print(cm);
            _display.print("cm");
        }

        _display.display();
    }

private:
    Adafruit_SSD1306 _display{128, 64, &Wire, -1};
};

#endif // DISPLAY_H
