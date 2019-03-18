/* This file was automatically generated.  Do not edit! */
void working_loop(char hostname);
void reconnect(char hostname);
void setup_wifi(char hostname);
void working_setup(char hostname);
extern WiFiClient espClient;
PubSubClient mqttClient(espClient);
extern const char *mqtt_server;
extern const char *password;
extern const char *ssid;
