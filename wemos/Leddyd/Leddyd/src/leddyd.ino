#include <ArduinoOTA.h>
#include <Ticker.h>
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "wifiConfig.h"

#define RELAY 4
//Les différents topics suivis
#define RGB_TOPIC  "color/plafond"
#define ANIMATION_TOPIC "animation/plafond"
#define OPEN_TOPIC  "switch/opinator/levier"
#define DOOR_TOPIC  "color/porte"
#define REFRESH_RATE 1

const char* hostname = "Leddyd1";
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
            boolean res  = mqttClient.subscribe(RGB_TOPIC, 1);
            boolean res2  = mqttClient.subscribe(ANIMATION_TOPIC, 1);
            boolean res3 = mqttClient.subscribe(DOOR_TOPIC, 1);
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
      analogWrite(GREEN_DOOR, LOW);
      analogWrite(RED,   LOW);
      analogWrite(GREEN, LOW);
      analogWrite(BLUE,  LOW);
    }else{
      IsOn = true;
      lastState    = 0;
      lastColor[0] = 0;
      lastColor[1] = 0;
      lastColor[2] = 0;
    }
  }
  else if(strcmp(topic, RGB_TOPIC) == 0){
    animationStarted = false;
    digitalWrite(LED_BUILTIN, HIGH);
    nextColor[0] = (double)payload[0];
    nextColor[1] = (double)payload[1];
    nextColor[2] = (double)payload[2];
  }
  else if(strcmp(topic, DOOR_TOPIC) == 0){
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

  //Mets les vraies valeurs de sortie à jour toutes les 10 millisecondes pour avoir une transition "propre" entre les états
  if(millis()%10 == 0){

    if(animationStarted){
      double chose1 = abs((int)(255.0*sin((double)animStep/1000)));
      double chose2 = abs((int)(255.0*sin(((double)animStep+5000)/1000)));
      double chose3 = abs((int)(255.0*sin(((double)animStep+10000)/1000)));

      if(IsOn) analogWrite(RED,   MAXF*chose1/255.0);
      if(IsOn) analogWrite(GREEN, MAXF*chose2/255.0);
      if(IsOn) analogWrite(BLUE,  MAXF*chose3/255.0);
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

      if(IsOn) analogWrite(RED,   MAXF*lastColor[0]/255.0);
      if(IsOn) analogWrite(GREEN, MAXF*lastColor[1]/255.0);
      if(IsOn) analogWrite(BLUE,  MAXF*lastColor[2]/255.0);
    }

    if(lastState != nextState){
      if(lastState > nextState){
        lastState --;
      }else if(lastState < nextState){
        lastState ++;
      }
      if(IsOn) analogWrite(GREEN_DOOR, MAXF*lastState/255.0);
    }
  }

  if (!mqttClient.connected()) {
      reconnect();
  }
  mqttClient.loop();
  ArduinoOTA.handle();
}
