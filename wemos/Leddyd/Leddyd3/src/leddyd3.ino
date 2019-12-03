/*
    Leddyd3 :
    Contrôle :
    - Strip RGB Derrière le panneau alcool
    - Strip Verte dans le couloir
*/

#include <ArduinoOTA.h>
#include <Ticker.h>
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "Wifi_config.h"

#define RELAY 4
//Les différents topics suivis
#define ALCOOL_TOPIC "color/alcool"
#define ANIMATION_TOPIC "animation/alcool"
#define OPEN_TOPIC  "switch/opinator/levier"
#define EXTERIOR_TOPIC "color/exterior"
#define REFRESH_RATE 1

const char* hostname = "Leddyd3";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";

#define ALCOOL_B D5
#define ALCOOL_R D6
#define ALCOOL_G D7
#define EXTERIOR_PIN D4

#define MAXF 1024

int relay_state = LOW;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Ticker refresh;
Ticker off;

bool IsOn = true;
double lastColor[3];
double nextColor[3];
double lastState = 0;
double nextState = 255;
bool animationStarted = false;
long int animStep = 0;

void reconnect() {
    String message = String(hostname) + " " + WiFi.localIP().toString() + " " + WiFi.macAddress();
    while (!mqttClient.connected()) {
        if (mqttClient.connect(hostname)) {
            mqttClient.publish("connect", message.c_str());
            boolean res  = mqttClient.subscribe(ALCOOL_TOPIC, 1);
            boolean res2  = mqttClient.subscribe(ANIMATION_TOPIC, 1);
            boolean res3  = mqttClient.subscribe(EXTERIOR_TOPIC, 1);
            boolean res4 = mqttClient.subscribe(OPEN_TOPIC, 1);
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
      analogWrite(ALCOOL_R,     LOW);
      analogWrite(ALCOOL_G,     LOW);
      analogWrite(ALCOOL_B,     LOW);
      analogWrite(EXTERIOR_PIN, LOW);
    }else{
      IsOn = true;
      lastColor[0] = 0;
      lastColor[1] = 0;
      lastColor[2] = 0;
      lastState = 0;
    }
  }
  else if(strcmp(topic, ALCOOL_TOPIC) == 0){
    animationStarted = false;
    nextColor[0] = (double)payload[0];
    nextColor[1] = (double)payload[1];
    nextColor[2] = (double)payload[2];
  }
  else if(strcmp(topic, EXTERIOR_TOPIC) == 0){
    nextState = (double)payload[0];
  }
  else if(strcmp(topic, ANIMATION_TOPIC) == 0){
    if(!animationStarted && (int)payload[0] == 255){
      animationStarted = true;
      digitalWrite(LED_BUILTIN, LOW);
      animStep = 0;
    }
  }
}

void setup(){
  //Initialise les pins du wemos
  pinMode(ALCOOL_R, OUTPUT);
  pinMode(ALCOOL_G ,OUTPUT);
  pinMode(ALCOOL_B ,OUTPUT);
  pinMode(EXTERIOR_PIN, OUTPUT);

  setup_wifi();
  ArduinoOTA.begin();
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqtt_callback);
}

void loop(){

  //Mets les vraies valeurs de sortie à jour toutes les 10 millisecondes pour avoir une transition "propre" entre les états
  if(millis()%10 == 0){
    if(animationStarted){
      double chose1 = abs((int)(255.0*sin((double)animStep/1000)));
      double chose2 = abs((int)(255.0*sin(((double)animStep+5000)/1000)));
      double chose3 = abs((int)(255.0*sin(((double)animStep+10000)/1000)));

      if(IsOn) analogWrite(ALCOOL_R,   MAXF*chose1/255.0);
      if(IsOn) analogWrite(ALCOOL_G, MAXF*chose2/255.0);
      if(IsOn) analogWrite(ALCOOL_B,  MAXF*chose3/255.0);
      animStep ++;
    }
    else{
      for(int x=0;x<3;x++){
        if(lastColor[x] != nextColor[x]){
          if(lastColor[x] > nextColor[x]){
            lastColor[x]--;
          }else if(lastColor[x] < nextColor[x]){
            lastColor[x]++;
          }
        }
      }
      if(IsOn) analogWrite(ALCOOL_R,   MAXF*lastColor[0]/255.0);
      if(IsOn) analogWrite(ALCOOL_G, MAXF*lastColor[1]/255.0);
      if(IsOn) analogWrite(ALCOOL_B,  MAXF*lastColor[2]/255.0);

    }
    if(nextState != lastState){
      if(lastState > nextState){
        lastState--;
      }else if(lastState < nextState){
        lastState++;
      }
      if(IsOn) analogWrite(EXTERIOR_PIN, MAXF*lastState/255.0);
    }
  }

  if (!mqttClient.connected()) {
      reconnect();
  }
  mqttClient.loop();
  ArduinoOTA.handle();
}
