/*
  
  Simple Arduino Sketch to publish data from Elsner P03/3-RS485 basic / Eltako Multisensor MS through a REST API

*/

#include "Arduino_Eltako-Multisensor-MS.h"


void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println(F("Eltako Multisensor MS started."));

  setup_sensor();

  setup_webserver();
}


void loop() {

  loop_sensor();

  loop_webserver();
  
}

