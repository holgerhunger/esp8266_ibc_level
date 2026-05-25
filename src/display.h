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

    void setPower(bool on) {
        _display.ssd1306_command(on ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
    }

    void show(int pct, bool wifiOk) {
        _display.clearDisplay();

        _display.setTextSize(1);
        _display.setCursor(0, 0);
        _display.print(wifiOk ? "WiFi OK" : "no WiFi");

        _display.setTextSize(3);
        _display.setCursor(10, 22);
        if (pct < 0) {
            _display.print("---");
        } else {
            _display.print(pct);
            _display.print("%");
        }

        _display.display();
    }

private:
    Adafruit_SSD1306 _display{128, 64, &Wire, -1};
};

#endif // DISPLAY_H
