/*
  
  Simple Sketch to publish data from Elsner P03/3-RS485 basic / Eltako Multisensor MS through a REST API

  Currently working on Arduino Uno with Ethernet Shield

*/

#include "Arduino_Elsner-Eltako-Weather-Sensor-RS485.h"


void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println(F("Elsner-Eltako-Weather-Sensor-RS485 started."));
  Serial.println();

  setup_sensor();

  setup_webserver();
}


void loop() {

  loop_sensor();

  loop_webserver();
  
}
