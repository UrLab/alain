#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include <ArduinoOTA.h>
#include "../wifiConfig.h"
#include <random>

#define BUTON D1
#define ANALOGIQUE A0

const char* hostname = "bullshit buton";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";
const int a = 0;

int state = HIGH;
String dtopic[6] = {"alcool", "plafond", "porte", "exterior", "radioactif", "infrabel"};

WiFiClient espClient;
PubSubClient mqttClient(espClient);
void setup_wifi();

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(BUTON, INPUT_PULLUP);
    pinMode(ANALOGIQUE, INPUT);
    srand(analogRead(ANALOGIQUE));

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
        if (mqttClient.connect(hostname)) {
            mqttClient.publish("connect", message.c_str());
        } else {
            delay(5000);
        }
    }
}

void open() {
    if(digitalRead(BUTON) == HIGH && digitalRead(BUTON) != state) {
        int yolo = 0;
        char rgb[3];
        rgb[0] = *static_cast<char*>(static_cast<void*>(&yolo));
        rgb[1] = *static_cast<char*>(static_cast<void*>(&yolo));
        rgb[2] = *static_cast<char*>(static_cast<void*>(&yolo));
        mqttClient.publish("animation/plafond", rgb , true);
        mqttClient.publish("animation/alcool", rgb , true);
    }
    state = digitalRead(BUTON);
    if (state == HIGH) {
        String topic = "color/"+ dtopic[rand()%6];
        if(topic != "color/alcool" && topic != "color/plafond") {
            int color_random = rand()%256;
            mqttClient.publish(topic.c_str(), static_cast<char*>(static_cast<void*>(&color_random)), true);
        }
        else {
            int r_random = rand()%256;
            int g_random = rand()%256;
            int b_random = rand()%256;
            char rgb[3];
            rgb[0] = *static_cast<char*>(static_cast<void*>(&r_random));
            rgb[1] = *static_cast<char*>(static_cast<void*>(&g_random));
            rgb[2] = *static_cast<char*>(static_cast<void*>(&b_random));
            mqttClient.publish(topic.c_str(), rgb, true);
            delay(50);
        }
  }
}

void loop() {
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();
    ArduinoOTA.handle();
    open();
}
