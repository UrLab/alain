#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include <ArduinoOTA.h>
#include "../../wifiConfig.h"
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN D8
#define DHTTYPE DHT11

const char* temperatureTopic = "sensors/temperature";
const char* humidityTopic = "sensors/humidity";

const char* hostname = "meteo";
const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "hal1.lan";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

DHT_Unified dht(DHTPIN, DHTTYPE);

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

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    setup_wifi();
    ArduinoOTA.begin();
    mqttClient.setServer(mqtt_server, 1883);
    Serial.begin(9600);
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

void loop() {
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();
    ArduinoOTA.handle();

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        Serial.println(F("Error reading temperature!"));
    }
    else {
        mqttClient.publish(temperatureTopic, String(event.temperature).c_str());
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        Serial.println(F("Error reading humidity!"));
    }
    else {
        mqttClient.publish(humidityTopic, String(event.relative_humidity).c_str());
    }
    
  delay(30000);  // Wait 30 seconds before measuring again
}
