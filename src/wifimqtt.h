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
String clientID = "ESP8266-";
// String clientID = "ESP32-";
WiFiClient espClient;
PubSubClient client(espClient);

inline void connectAP() {
    digitalWrite(LED, LED_ON);
    //Serial.print("Verbinde mit WLAN Netzwerk SSID: ");
    //Serial.print(WLAN_SSID);
    //Serial.print(" ");
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
    byte cnt = 0;
    while (WiFi.status() != WL_CONNECTED) {
        //Serial.print("*");
        cnt++;
        if (cnt > 30) { ESP.restart(); }
        delay(WLAN_CONNECT_WAIT);
    }
    //Serial.println(" WLAN verbunden!");
    //Serial.print("Lokale IP Adresse: ");
    //Serial.println(WiFi.localIP());
    digitalWrite(LED, LED_OFF);
}

inline void reconnect() {
    byte cnt = 0;
    while (!client.connected()) {
        digitalWrite(LED, LED_ON);
        //Serial.print("Verbinde mit MQTT Broker .. ");
        clientID += String(random(0xffff), HEX);
        if (client.connect(clientID.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            //Serial.println("Mit MQTT verbunden!");
            digitalWrite(LED, LED_OFF);
            // client.subscribe(MQTT_TOPIC);
            // client.subscribe("ledState");
            // client.subscribe("testJSON");
        } else {
            //Serial.print("fehlgeschlagen, rc=");
            //Serial.print(client.state());
            //Serial.println(" versuche es in 5 Sek nochmal.");
            digitalWrite(LED, LED_ON);
            if (WiFi.status() != WL_CONNECTED) { connectAP(); }
            cnt++;
            if (cnt > 10) { ESP.restart(); }
            delay(5000);
        }
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
