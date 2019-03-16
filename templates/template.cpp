#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include <ArduinoOTA.h>

const char* hostname = "MY_HOSTNAE(folder_name)";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    setup_wifi();
    ArduinoOTA.begin();
    mqttClient.setServer(mqtt_server, 1883);
}

void setup_wifi() {
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

void reconnect() {
    String message = String(hostname) + " " + WiFi.localIP().toString() + " " + WiFi.macAddress();
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP8266Client")) {
            mqttClient.publish("connect", message.c_str());
        } else {
            delay(5000);
        }
    }
}

void loop() {
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();
    ArduinoOTA.handle();
}
