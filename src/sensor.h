#ifndef SENSOR_H
#define SENSOR_H

#include <Wire.h>
#include <VL53L0X.h>

class IbcSensor {
public:
    bool begin() {
        _sensor.setTimeout(500);
        if (!_sensor.init()) {
            Serial.println("Failed to detect and initialize sensor!");
            return false;
        }
        // lower the return signal rate limit (default is 0.25 MCPS)
        _sensor.setSignalRateLimit(0.1);
        // increase laser pulse periods (defaults are 14 and 10 PCLKs)
        _sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
        _sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
        _sensor.setMeasurementTimingBudget(200000);
        return true;
    }

    // Gibt Abstand in cm zurück, -1 bei Fehler
    int readCm() {
        // Versuch die Daten zu glätten
        int data;
        int mm = 0;
        for (int i = 0; i < 15; i++) {
            delay(2);
            data = _sensor.readRangeSingleMillimeters();
            if (data == 8190) { continue; }
            if (mm == 0) { mm = data; } else { mm = (mm + data) / 2; }
        }

        if (_sensor.timeoutOccurred()) {
            Serial.println("Sensor: kein Signal");
            return -1;
        }

        int cm = (mm - CALIBRATION_OFFSET_MM) / 10;
        Serial.print("Sensor: ");
        Serial.print(cm);
        Serial.print(" cm   ");
        Serial.print(mm);
        Serial.println(" mm");
        return cm;
    }

private:
    static constexpr int CALIBRATION_OFFSET_MM = 0; // Sensor liest systematisch 5 cm zuviel
    VL53L0X _sensor;
};

#endif // SENSOR_H
