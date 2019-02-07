#include <Ticker.h>
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include <ArduinoOTA.h>
#include "../../../../wifiConfig.h"

#define RELAY 4 // D2
#define OPEN_TOPIC  "events/open"
#define REFRESH_RATE 1

const char* hostname = "MarieGuiGui";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal.lan";


int relay_state = LOW;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Ticker refresh;
Ticker off;

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
            off.once((float)120, off_callback);
        }
        else if(payload[0] == '1'){
            relay_state = HIGH;
            off.detach();
        }
    }
}

void refresh_callback() {
    digitalWrite(RELAY, relay_state);
}

void off_callback()  {
    relay_state = LOW;
}

void setup() {
    pinMode(RELAY, OUTPUT);
    digitalWrite(RELAY, relay_state);

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
