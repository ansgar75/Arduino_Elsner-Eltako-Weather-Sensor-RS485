#include "Arduino_Elsner-Eltako-Weather-Sensor-RS485.h"

#include <SoftwareSerial.h>


SoftwareSerial sensorSerial(8, 9); // RX, TX (dummy, not used here)

String sensorMessage = "";
int sensorMessageChecksum = 0;

SensorData sensor_data;
WindSpeedBuffer wind_speed_history_buffer;


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
          sensor_data.temperature_tenth_c = sensorMessage.substring(1, 4).toInt() * 10 + sensorMessage.substring(5, 6).toInt();
          sensor_data.sun_south_klx = sensorMessage.substring(6, 8).toInt();
          sensor_data.sun_west_klx = sensorMessage.substring(8, 10).toInt();
          sensor_data.sun_east_klx = sensorMessage.substring(10, 12).toInt();
          sensor_data.is_dark = sensorMessage[12] == 'J';
          sensor_data.daylight_lx = sensorMessage.substring(13, 16).toInt();
          sensor_data.wind_speed_tenth_ms = sensorMessage.substring(16, 18).toInt() * 10 + sensorMessage.substring(19, 20).toInt();
          sensor_data.is_raining = sensorMessage[20] == 'J';

          wind_speed_history_buffer.value[wind_speed_history_buffer.position] = sensor_data.wind_speed_tenth_ms;
          wind_speed_history_buffer.position++;
          if (wind_speed_history_buffer.position >= WIND_SPEED_BUFFER_SIZE) {
            wind_speed_history_buffer.position = 0;
          }

          sensor_data.wind_speed_gusts_tenth_ms = 0;
          for (int i = 0; i < WIND_SPEED_BUFFER_SIZE; i++) {
            if (wind_speed_history_buffer.value[i] > sensor_data.wind_speed_gusts_tenth_ms) {
              sensor_data.wind_speed_gusts_tenth_ms = wind_speed_history_buffer.value[i];
            }
          }

          Serial.print(F("readingAgeMs: "));
          Serial.print(millis() - sensor_data.timestamp_millis);
          Serial.print(F(", temperatureTenthC: "));
          Serial.print(sensor_data.temperature_tenth_c);
          Serial.print(F(", sunSouthKlx: "));
          Serial.print(sensor_data.sun_south_klx);
          Serial.print(F(", sunWestKlx: "));
          Serial.print(sensor_data.sun_west_klx);
          Serial.print(F(", sunEastKlx: "));
          Serial.print(sensor_data.sun_east_klx);
          Serial.print(F(", isDark: "));
          Serial.print(sensor_data.is_dark);
          Serial.print(F(", daylightLx: "));
          Serial.print(sensor_data.daylight_lx);
          Serial.print(F(", windSpeedTenthMs: "));
          Serial.print(sensor_data.wind_speed_tenth_ms);
          Serial.print(F(", windSpeedGustsTenthMs: "));
          Serial.print(sensor_data.wind_speed_gusts_tenth_ms);
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

