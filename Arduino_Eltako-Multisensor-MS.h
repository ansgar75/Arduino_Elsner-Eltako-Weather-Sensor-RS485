#ifndef Arduino_Eltako_Multisensor_MS_h
#define Arduino_Eltako_Multisensor_MS_h

#include <Arduino.h>


typedef struct
{
  long timestamp_millis;
  float temperature_c;
  int sun_south_klx;
  int sun_west_klx;
  int sun_east_klx;
  bool is_twilight;
  int daylight_lx;
  float wind_speed_ms;
  bool is_raining;
} SensorData;

extern SensorData sensor_data;


void setup_sensor();
void setup_webserver();

void loop_sensor();
void loop_webserver();


#endif  // Arduino_Eltako_Multisensor_MS_h