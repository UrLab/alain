#include <ArduinoOTA.h>
#include <Ticker.h>
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "../include/Wifi_config.h"

#define RELAY 4
#define RGB_TOPIC  "color/plafond"
#define OPEN_TOPIC  "switch/opinator/levier"
#define REFRESH_RATE 1

const char* hostname = "Leddyd";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";

#define RED D3
#define GREEN D2
#define BLUE D1
#define GREEN_DOOR D5

#define MAXF 1024

int relay_state = LOW;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Ticker refresh;
Ticker off;

//pio init -b d1_mini_lite
//pio run --target upload

bool IsOn = true;
char lastColor[3];

void reconnect() {
    String message = String(hostname) + " " + WiFi.localIP().toString() + " " + WiFi.macAddress();
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP8266Client")) {
            mqttClient.publish("connect", message.c_str());
            boolean res  = mqttClient.subscribe(RGB_TOPIC, 1);
            boolean res2 = mqttClient.subscribe(OPEN_TOPIC, 1);
        } else {
            delay(5000);
        }
    }
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

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic, OPEN_TOPIC) == 0){
    if(payload[0] == '0'){
      analogWrite(GREEN_DOOR, LOW);
      IsOn = false;
      analogWrite(RED,   LOW);
      analogWrite(GREEN, LOW);
      analogWrite(BLUE,  LOW);
    }else{
      analogWrite(GREEN_DOOR, MAXF/2);
      IsOn = true;
      analogWrite(RED,   MAXF*(double)lastColor[0]/255.0);
      analogWrite(GREEN, MAXF*(double)lastColor[1]/255.0);
      analogWrite(BLUE,  MAXF*(double)lastColor[2]/255.0);
    }
  }else{
    lastColor[0] = payload[0];
    lastColor[1] = payload[1];
    lastColor[2] = payload[2];
    if (IsOn){
      analogWrite(RED,   MAXF*(double)payload[0]/255.0);
      analogWrite(GREEN, MAXF*(double)payload[1]/255.0);
      analogWrite(BLUE,  MAXF*(double)payload[2]/255.0);
    }
  }
}

void setup(){
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN_DOOR, OUTPUT);

  setup_wifi();
  ArduinoOTA.begin();
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqtt_callback);
}

void loop(){
  if (!mqttClient.connected()) {
      reconnect();
  }
  mqttClient.loop();
  ArduinoOTA.handle();
}
