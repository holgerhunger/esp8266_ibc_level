#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include "globals.h"

class IbcSensor {
public:
    bool begin() {
        pinMode(TRIGGER_PIN, OUTPUT);
        pinMode(ECHO_PIN, INPUT);
        digitalWrite(TRIGGER_PIN, LOW);
        delay(50);
        return true;
    }

    // Gibt Abstand in cm zurück, -1 bei Fehler
    int readCm() {
        long total = 0;
        int count = 0;

        for (int i = 0; i < SAMPLES; i++) {
            long cm = measure();
            if (cm > 0) {
                total += cm;
                count++;
            }
            delay(30);
        }

        if (count == 0) {
            Serial.println("Sensor: kein Echo");
            return -1;
        }

        int cm = total / count;
        Serial.print("Sensor: ");
        Serial.print(cm);
        Serial.println(" cm");
        return cm;
    }

private:
    static constexpr int SAMPLES = 5;

    long measure() {
        digitalWrite(TRIGGER_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIGGER_PIN, LOW);

        // Timeout 30ms entspricht ~5m, mehr als genug für IBC
        long duration = pulseIn(ECHO_PIN, HIGH, 30000);
        if (duration == 0) return 0;

        return duration / 58;  // µs → cm (Schall hin+zurück: 343m/s → /58.3)
    }
};

#endif // SENSOR_H
