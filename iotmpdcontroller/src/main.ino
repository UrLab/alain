#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include <ArduinoOTA.h>
#include "../../../../wifiConfig.h"

#define PLAY_BUTTON 4
#define MINUS_BUTTON 5
#define PLUS_BUTTON 0

const char* hostname = "DJcrazyPlayButton";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";

unsigned long lastPlayInterrupt = 0;
unsigned long lastPlusInterrupt = 0;
unsigned long lastMinusInterrupt = 0;

int lastPlaystate = HIGH;
int lastPlusstate = HIGH;
int lastMinusstate = HIGH;


WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
    pinMode(PLAY_BUTTON, INPUT);
    pinMode(MINUS_BUTTON, INPUT);
    pinMode(PLUS_BUTTON, INPUT);
    attachInterrupt(digitalPinToInterrupt(PLAY_BUTTON), playInterupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MINUS_BUTTON), minusInterupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PLUS_BUTTON), plusInterupt, CHANGE);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    setup_wifi();
    ArduinoOTA.begin();

    mqttClient.setServer(mqtt_server, 1883);
    Serial.begin(9600);
}

void setup_wifi() {
    delay(10);

    String superHostname = String("hal-") + hostname;
    WiFi.mode(WIFI_STA);
    WiFi.hostname(superHostname);
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

void playInterupt(void) {
    int state = digitalRead(PLAY_BUTTON);
    if(state == lastPlaystate)
        return;
    lastPlaystate = state;
    if(state == HIGH){
        unsigned long delta = millis() - lastPlayInterrupt;
        String actionTopic = String("action/") + hostname + "/playButton";
        if(delta > 1000){
            mqttClient.publish("music/control", "stop");
            mqttClient.publish(actionTopic.c_str(), "longpress");
        } else if (delta > 5) {
            mqttClient.publish("music/control", "toggle");
            mqttClient.publish(actionTopic.c_str(), "press");
        }
    }
    lastPlayInterrupt = millis();
}

void plusInterupt(void) {
    int state = digitalRead(PLUS_BUTTON);
    if(state == lastPlusstate)
        return;
    lastPlusstate = state;
    if(state == HIGH){
        unsigned long delta = millis() - lastPlusInterrupt;
        String actionTopic = String("action/") + hostname + "/plusButton";
        if(delta > 1000){
            mqttClient.publish("music/control", "next");
            mqttClient.publish(actionTopic.c_str(), "longpress");
        } else if (delta > 5) {
            mqttClient.publish("music/volume", "+5");
            mqttClient.publish(actionTopic.c_str(), "press");
        }
    }
    lastPlusInterrupt = millis();
}

void minusInterupt(void) {
    int state = digitalRead(MINUS_BUTTON);
    if(state == lastMinusstate)
        return;
    lastMinusstate = state;
    if(state == HIGH){
        unsigned long delta = millis() - lastMinusInterrupt;
        String actionTopic = String("action/") + hostname + "/minusButton";
        if(delta > 1000){
            mqttClient.publish("music/control", "previous");
            mqttClient.publish(actionTopic.c_str(), "longpress");
        } else if (delta > 5) {
            mqttClient.publish("music/volume", "-5");
            mqttClient.publish(actionTopic.c_str(), "press");
        }
    }
    lastMinusInterrupt = millis();
}

void loop() {
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();
    ArduinoOTA.handle();
}
