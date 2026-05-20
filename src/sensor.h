#ifndef SENSOR_H
#define SENSOR_H

#include <Adafruit_VL53L0X.h>

class IbcSensor {
public:
    bool begin() {
        if (!_sensor.begin()) {
            Serial.println("VL53L0X nicht gefunden");
            return false;
        }
        return true;
    }

    // Gibt Abstand in cm zurück, -1 bei Fehler
    int readCm() {
        VL53L0X_RangingMeasurementData_t measure;
        _sensor.rangingTest(&measure, false);
        if (measure.RangeStatus != 0) {
            Serial.println("Sensor: kein Signal");
            return -1;
        }
        int cm = (measure.RangeMilliMeter - CALIBRATION_OFFSET_MM) / 10;
        Serial.print("Sensor: ");
        Serial.print(cm);
        Serial.println(" cm");
        return cm;
    }

private:
    static constexpr int CALIBRATION_OFFSET_MM = 50; // Sensor liest systematisch 5 cm zuviel
    Adafruit_VL53L0X _sensor;
};

#endif // SENSOR_H
