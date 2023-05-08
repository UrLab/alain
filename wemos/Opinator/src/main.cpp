#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include "../../wifiConfig.h"

const char* hostname = "opinator";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal1.lan";

// #define LED D0
#define BOUTON D1
#define LED D0

WiFiClient espClient;
PubSubClient mqttClient(espClient);

int state;
int last_state;


void setup_wifi() {
    delay(10);

    WiFi.mode(WIFI_STA);
    WiFi.hostname(hostname);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("wifi ");
        Serial.println(WiFi.status());
    }
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

void setup() {
    pinMode(BOUTON, INPUT_PULLUP);
    pinMode(LED, OUTPUT);

    setup_wifi();
    mqttClient.setServer(mqtt_server, 1883);
}

void open() {
    state = digitalRead(BOUTON);
    if (last_state!=state){
        if (state == HIGH){
            mqttClient.publish(hostname, "1", true);
            digitalWrite(LED, HIGH);
        }
        else{
            mqttClient.publish(hostname, "0", true);
            digitalWrite(LED, LOW);
        }
    }
    last_state = state;
    delay(500);
}


void loop() {
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();
    open();
}
