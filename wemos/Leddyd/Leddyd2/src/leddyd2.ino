#include <ArduinoOTA.h>
#include <Ticker.h>
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "../include/Wifi_config.h"

#define RELAY 4
//Les différents topics suivis
#define RADIOACTIF_TOPIC "color/radioactif"
#define INFRABEL_TOPIC "color/infrabel"
#define OPEN_TOPIC  "switch/opinator/levier"
#define NOEILDROIT_TOPIC  "color/nyeunyeudroit"
#define NOEILGAUCHE_TOPIC  "color/nyeunyeugauche"
#define REFRESH_RATE 1

const char* hostname = "Leddyd2";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";

#define INFRA D5
#define RADIO D6
#define NOEILDROIT D7
#define NOEILGAUCHE D8

#define MAXF 1024

int relay_state = LOW;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Ticker refresh;
Ticker off;

//pio init -b d1_mini_lite
//pio run --target upload

bool IsOn = true;
double currentStateInfra = 0;
double currentStateRadio = 0;
double nextStateInfra = 255;
double nextStateRadio = 255;

double nextStateDroit = 255;
double nextStateGauche = 255;
double currentStateDroit = 0;
double currentStateGauche = 0;

void reconnect() {
    String message = String(hostname) + " " + WiFi.localIP().toString() + " " + WiFi.macAddress();
    while (!mqttClient.connected()) {
        if (mqttClient.connect(hostname)) {
            mqttClient.publish("connect", message.c_str());
            boolean res  = mqttClient.subscribe(RADIOACTIF_TOPIC, 1);
            boolean res2 = mqttClient.subscribe(INFRABEL_TOPIC, 1);
            boolean res3 = mqttClient.subscribe(OPEN_TOPIC, 1);
            boolean res4 = mqttClient.subscribe(NOEILDROIT_TOPIC, 1);
            boolean res5 = mqttClient.subscribe(NOEILGAUCHE_TOPIC, 1);
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

//Mets à jour les valeurs de sortie des pins du wemos en fonction du topic sur lequel le message à été recu
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic, OPEN_TOPIC) == 0){
    if(payload[0] == '0'){
      IsOn = false;
      analogWrite(INFRA, LOW);
      analogWrite(RADIO, LOW);
    }else{
      IsOn = true;
      currentStateInfra = 0;
      currentStateRadio = 0;
    }
  }else if(strcmp(topic, RADIOACTIF_TOPIC) == 0){
    nextStateRadio = (double)payload[0];
  }else if(strcmp(topic, INFRABEL_TOPIC) == 0){
    nextStateInfra = (double)payload[0];
  }else if(strcmp(topic, NOEILDROIT_TOPIC) == 0){
    nextStateDroit = (double)payload[0];
  }else if(strcmp(topic, NOEILGAUCHE_TOPIC) == 0){
    nextStateGauche = (double)payload[0];
  }
}

void setup(){
  //Initialise les pins du wemos
  pinMode(INFRA, OUTPUT);
  pinMode(RADIO ,OUTPUT);

  setup_wifi();
  ArduinoOTA.begin();
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqtt_callback);
}

void loop(){

    //Mets les vraies valeurs de sortie à jour toutes les 20 millisecondes pour avoir une transition "propre" entre les états
  if(millis()%20 == 0){
    if(nextStateInfra != currentStateInfra){
      if(nextStateInfra > currentStateInfra){
        currentStateInfra++;
      }else if(nextStateInfra < currentStateInfra){
        currentStateInfra --;
      }
      if(IsOn) analogWrite(INFRA, currentStateInfra);
    }

    if(nextStateRadio != currentStateRadio){
      if(nextStateRadio > currentStateRadio){
        currentStateRadio++;
      }else if(nextStateRadio < currentStateRadio){
        currentStateRadio --;
      }
      if(IsOn) analogWrite(RADIO, currentStateRadio);
    }

    if(nextStateDroit != currentStateDroit){
      if(nextStateDroit > currentStateDroit){
        currentStateDroit++;
      }else if(nextStateDroit < currentStateDroit){
        currentStateDroit --;
      }
      if(IsOn) analogWrite(NOEILDROIT, currentStateDroit);
    }

    if(nextStateGauche != currentStateGauche){
      if(nextStateGauche > currentStateGauche){
        currentStateGauche++;
      }else if(nextStateGauche < currentStateGauche){
        currentStateGauche --;
      }
      if(IsOn) analogWrite(NOEILGAUCHE, currentStateGauche);
    }
  }

  if (!mqttClient.connected()) {
      reconnect();
  }
  mqttClient.loop();
  ArduinoOTA.handle();
}
