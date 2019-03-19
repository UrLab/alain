#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include <ArduinoOTA.h>
#include "../../wifiConfig.h"

const char* hostname = "hostname a changer";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

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

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    setup_wifi();
    ArduinoOTA.begin();
    mqttClient.setServer(mqtt_server, 1883);
}

void reconnect() {
    String message = String(hostname) + " " + WiFi.localIP().toString() + " " + WiFi.macAddress();
    while (!mqttClient.connected()) {
        if (mqttClient.connect(hostname)) {
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
