#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include <ArduinoOTA.h>

#define LEVIER D1

const char* hostname = "opinator";
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "hal.lan";
const int a = 0;

unsigned long last_change_time = 0 ;

int last_state = HIGH;
int state = HIGH;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
void setup_wifi();
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(LEVIER, INPUT_PULLUP);

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
  state = digitalRead(LEVIER);
  String topic = "switch/"+ String(hostname) + "/levier";
  if (state != last_state && (millis()) - last_change_time  > 500) {
    last_change_time = millis();
    mqttClient.publish(topic.c_str(), (state == HIGH) ? "0" : "1", true);
    last_state = state;
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
