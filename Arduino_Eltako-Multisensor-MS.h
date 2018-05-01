#ifndef Arduino_Eltako_Multisensor_MS_h
#define Arduino_Eltako_Multisensor_MS_h

#include <Arduino.h>


typedef struct
{
  long timestamp;
  float at;
  int sos;
  int sow;
  int soo;
  bool daemmerung;
  int tageslicht;
  float wind;
  bool regen;
} SensorData;


void setup_sensor();
void loop_sensor();

#endif  // Arduino_Eltako_Multisensor_MS_h

