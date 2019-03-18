#include <ArduinoOTA.h>
#include "../../template.cpp"

char hostname = 'Le hostname a mettre';

void setup() {
  working_setup(hostname);
}

void loop() {
  working_loop(hostname);
}
