#include "Arduino_Eltako-Multisensor-MS.h"

#include <SoftwareSerial.h>


SoftwareSerial sensorSerial(8, 9); // RX, TX (dummy, not used here)

String sensorMessage = "";
int sensorMessageChecksum = 0;

SensorData sensor_data;


void setup_sensor() {

  // Multisensor MS: 19200,8,N,1
  sensorSerial.begin(19200);

}


void loop_sensor() {

  if (sensorSerial.available()) {

    char sensorData = sensorSerial.read();

    // start of message from sensor?
    if (sensorData == 'W') {
      sensorMessage = "";
      sensorMessageChecksum = 0;
    }

    sensorMessage += sensorData;
    if (sensorMessage.length() <= 35) {
      sensorMessageChecksum += sensorData;
    }

    // end of message from sensor?
    if (sensorData == 0x03) {

      if (sensorMessage.length() == 40) {

        int checkSumSender = sensorMessage.substring(35, 39).toInt();
        if (sensorMessageChecksum == checkSumSender) {

          Serial.print(F("Received valid sensor message: "));
          Serial.println(sensorMessage);

          sensor_data.timestamp_millis = millis();
          sensor_data.temperature_c = sensorMessage.substring(1, 6).toFloat();
          sensor_data.sun_south_klx = sensorMessage.substring(6, 8).toInt();
          sensor_data.sun_west_klx = sensorMessage.substring(8, 10).toInt();
          sensor_data.sun_east_klx = sensorMessage.substring(10, 12).toInt();
          sensor_data.is_twilight = sensorMessage[12] == 'J';
          sensor_data.daylight_lx = sensorMessage.substring(13, 16).toInt();
          sensor_data.wind_speed_ms = sensorMessage.substring(16, 20).toFloat();
          sensor_data.is_raining = sensorMessage[20] == 'J';

          Serial.print(F("readingAgeMs: "));
          Serial.print(millis() - sensor_data.timestamp_millis);
          Serial.print(F(", temperatureC: "));
          Serial.print(sensor_data.temperature_c);
          Serial.print(F(", sunSouthKlx: "));
          Serial.print(sensor_data.sun_south_klx);
          Serial.print(F(", sunWestKlx: "));
          Serial.print(sensor_data.sun_west_klx);
          Serial.print(F(", sunEastKlx: "));
          Serial.print(sensor_data.sun_east_klx);
          Serial.print(F(", isTwilight: "));
          Serial.print(sensor_data.is_twilight);
          Serial.print(F(", daylightLx: "));
          Serial.print(sensor_data.daylight_lx);
          Serial.print(F(", windSpeedMs: "));
          Serial.print(sensor_data.wind_speed_ms);
          Serial.print(F(", isRaining: "));
          Serial.print(sensor_data.is_raining);
          Serial.println();

        } else {

          Serial.print(F("Invalid message checksum - calculated: "));
          Serial.print(sensorMessageChecksum);
          Serial.print(F(", received: "));
          Serial.print(checkSumSender);
          Serial.print(F(", message: "));
          Serial.print(sensorMessage);
          Serial.println();

        }

      } else {

        Serial.print(F("Invalid message length: "));
        Serial.print(sensorMessage.length());
        Serial.print(F(", message: "));
        Serial.print(sensorMessage);
        Serial.println();

      }
    }

  }

}
