#include <Ticker.h>
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include <ArduinoOTA.h>
#include "../../../../wifiConfig.h"

#define DUCK1 4 // D2
#define DUCK2 13 // D7
#define DUCK3 12 // D6
#define DUCK4 14 // D5


#define OPEN_TOPIC  "switch/opinator/levier"
#define REFRESH_RATE 1

const char* hostname = "duckass";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";


int relay_state = LOW;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Ticker refresh;

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

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    if(length > 0){
        if(payload[0] == '0'){
            relay_state = LOW;
        }
        else if(payload[0] == '1'){
            relay_state = HIGH;
        }
    }
}

void refresh_callback() {
    digitalWrite(DUCK1, relay_state);
    digitalWrite(DUCK2, relay_state);
    digitalWrite(DUCK3, relay_state);
    digitalWrite(DUCK4, relay_state);
}

void setup() {
    pinMode(DUCK1, OUTPUT);
    pinMode(DUCK2, OUTPUT);
    pinMode(DUCK3, OUTPUT);
    pinMode(DUCK4, OUTPUT);
    digitalWrite(DUCK1, relay_state);
    digitalWrite(DUCK2, relay_state);
    digitalWrite(DUCK3, relay_state);
    digitalWrite(DUCK4, relay_state);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    setup_wifi();
    ArduinoOTA.begin();

    mqttClient.setServer(mqtt_server, 1883);
    mqttClient.setCallback(mqtt_callback);
    Serial.begin(9600);
    refresh.attach(REFRESH_RATE, refresh_callback);
}

void reconnect() {
    String message = String(hostname) + " " + WiFi.localIP().toString() + " " + WiFi.macAddress();
    while (!mqttClient.connected()) {
        if (mqttClient.connect(hostname)) {
            mqttClient.publish("connect", message.c_str());
            boolean res =mqttClient.subscribe(OPEN_TOPIC, 1);
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
