#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include <Arduino.h>

// LDR5528 an A0: 3.3V — 100k — A0 — LDR — GND
// Hoher ADC-Wert = hoher LDR-Widerstand = dunkel
class LdrSensor {
public:
    void begin() {
        pinMode(A0, INPUT);
    }

    // Rohwert 0–1023 (0 = hell, 1023 = dunkel)
    int readRaw() {
        long total = 0;
        for (int i = 0; i < SAMPLES; i++) {
            total += analogRead(A0);
            delay(5);
        }
        int raw = total / SAMPLES;
        Serial.print("LDR: ");
        Serial.println(raw);
        return raw;
    }

private:
    static constexpr int SAMPLES = 4;
};

#endif // BRIGHTNESS_H
