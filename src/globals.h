//
// Created by holger on 17.05.26
// AZ-Delivery esp8266 D1 mini
//

#ifndef GLOBALS_H
#define GLOBALS_H

auto MQTT_TOPIC = "keller/ibc_level";

// I2C
#define SDA 4
#define SCL 5

// HC-SR04
#define TRIGGER_PIN 14
#define ECHO_PIN 13

// Onboard LED
#define LED 2
#define LED_ON LOW
#define LED_OFF HIGH

#define WLAN_CONNECT_WAIT 1000

#define DISPLAY_INTERVAL_MS  5000UL    // Mess- und Anzeigetakt wenn Display AN
#define MQTT_INTERVAL_MS     300000UL  // MQTT-Publish-Intervall (5 Minuten)

// LDR: ADC-Wert ab dem das Display abgeschaltet wird (dunkel = hoher Wert)
#define LDR_DARK_THRESHOLD 750

// IBC-Geometrie (cm)
#define TANK_SENSOR_HEIGHT_CM 106   // Abstand Behälterboden bis Sensor
#define TANK_OVERFLOW_CM      88    // Füllstand bei dem der Behälter überläuft

#endif //GLOBALS_H
