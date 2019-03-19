#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include <ArduinoOTA.h>
#include "wifiConfig.h"
#include "template.hpp"

// char* hostname = "MY_HOSTNAE(folder_name)";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void working_setup(char hostname) {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    setup_wifi(hostname);
    ArduinoOTA.begin();
    mqttClient.setServer(mqtt_server, 1883);
}

void setup_wifi(char hostname) {
    delay(10);

    String superHostname = String("hal-") + hostname;
    WiFi.mode(WIFI_STA);
    WiFi.hostname(superHostname);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void reconnect(char hostname) {
    String message = String(hostname) + " " + WiFi.localIP().toString() + " " + WiFi.macAddress();
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP8266Client")) {
            mqttClient.publish("connect", message.c_str());
        } else {
            delay(5000);
        }
    }
}

void working_loop(char hostname) {
    if (!mqttClient.connected()) {
        reconnect(hostname);
    }
    mqttClient.loop();
    ArduinoOTA.handle();
}
