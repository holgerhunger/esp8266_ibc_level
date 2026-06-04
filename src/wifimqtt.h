//
// Created by holger on 02.08.24.
//

#ifndef WIFIMQTT_H
#define WIFIMQTT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "myConfig.h"
#include "globals.h"

// MQTT
String clientID = "ESP8266-IBC";
// String clientID = "ESP32-";
WiFiClient espClient;
PubSubClient client(espClient);

// Beim Boot: WLAN-Verbindung versuchen, aber bei Misserfolg NICHT neu starten,
// damit das Gerät auch ohne WLAN hochfährt und das Display Werte anzeigt.
inline void connectAP() {
    digitalWrite(LED, LED_ON);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
    byte cnt = 0;
    while (WiFi.status() != WL_CONNECTED) {
        cnt++;
        if (cnt > 30) {            // nach ~30 s aufgeben statt Restart
            digitalWrite(LED, LED_OFF);
            return;
        }
        delay(WLAN_CONNECT_WAIT);
    }
    digitalWrite(LED, LED_OFF);
}

// Stellt WLAN (falls nötig) und MQTT mit jeweils max. 3 Versuchen her.
// Gelingt das nicht, wird ein Fehlversuch gezählt; erst nach dem 3. erfolglosen
// reconnect()-Aufruf in Folge erfolgt ein Neustart. Da reconnect() nur im
// MQTT-Intervall (5 Min) aufgerufen wird, startet das Gerät frühestens nach
// ~15 Min ohne Verbindung neu.
inline void reconnect() {
    static byte failedCalls = 0;

    // WLAN sicherstellen (max. 3 Versuche)
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
        for (byte i = 0; i < 3 && WiFi.status() != WL_CONNECTED; i++) {
            digitalWrite(LED, LED_ON);
            delay(WLAN_CONNECT_WAIT);
        }
    }

    // MQTT verbinden (max. 3 Versuche), nur wenn WLAN steht
    for (byte i = 0; i < 3 && WiFi.status() == WL_CONNECTED && !client.connected(); i++) {
        digitalWrite(LED, LED_ON);
        // frische Client-ID je Versuch (ohne den Basisnamen unbegrenzt wachsen zu lassen)
        String id = clientID + String(random(0xffff), HEX);
        if (client.connect(id.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            // client.subscribe(MQTT_TOPIC);
            // client.subscribe("ledState");
            // client.subscribe("testJSON");
            break;
        }
        if (i < 2) { delay(5000); }   // zwischen Versuchen warten, nach dem letzten nicht
    }

    if (client.connected()) {
        failedCalls = 0;
        digitalWrite(LED, LED_OFF);
    } else {
        failedCalls++;
        digitalWrite(LED, LED_OFF);
        if (failedCalls >= 3) { ESP.restart(); }
    }
}

inline void callback(char *topic, byte *message, unsigned int length) {
    //Serial.print("Message arrived on topic: ");
    //Serial.print(topic);
    //Serial.print(". Message: ");
    String messageTemp;
    for (int i = 0; i < length; i++) {
        messageTemp += static_cast<char>(message[i]);
    }
    //Serial.println(messageTemp);

    if (String(topic) == "test") {
        //Serial.print("From test topic : ");
        //Serial.println(messageTemp);
    } else if (String(topic) == "ledState") {
        if (messageTemp == "on") {
            digitalWrite(LED, LED_ON);
        } else if (messageTemp == "off") {
            digitalWrite(LED, LED_OFF);
        }
    } else if (String(topic) == "testJSON") {
        // Allocate the JSON document
        JsonDocument doc;

        // JSON input string.
        // const char* json = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

        // Deserialize the JSON document
        DeserializationError error = deserializeJson(doc, messageTemp);

        // Test if parsing succeeds.
        if (error) {
            //Serial.print(F("deserializeJson() failed: "));
            //Serial.println(error.f_str());
            return;
        }

        const char *device = doc["device"];
        int temperature = doc["temperature"];
        int humidity = doc["humidity"];
        int lux = doc["lux"];

        //Serial.print("From test topic : ");
        //Serial.print(device);
        //Serial.print(" ; ");
        //Serial.print(temperature);
        //Serial.print(" ; ");
        //Serial.print(humidity);
        //Serial.print(" ; ");
        //Serial.println(lux);
    }
}

#endif //WIFIMQTT_H
