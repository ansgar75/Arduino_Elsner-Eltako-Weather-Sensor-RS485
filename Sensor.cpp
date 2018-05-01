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

          Serial.println(sensorMessage);

          sensor_data.timestamp = millis();
          sensor_data.at = sensorMessage.substring(1, 6).toFloat();
          sensor_data.sos = sensorMessage.substring(6, 8).toInt();
          sensor_data.sow = sensorMessage.substring(8, 10).toInt();
          sensor_data.soo = sensorMessage.substring(10, 12).toInt();
          sensor_data.daemmerung = sensorMessage[12] == 'J';
          sensor_data.tageslicht = sensorMessage.substring(13, 16).toInt();
          sensor_data.wind = sensorMessage.substring(16, 20).toFloat();
          sensor_data.regen = sensorMessage[20] == 'J';

          Serial.print(F("Alter: "));
          Serial.print(millis() - sensor_data.timestamp);
          Serial.print(F(", Temp: "));
          Serial.print(sensor_data.at);
          Serial.print(F(", Sonne Sued: "));
          Serial.print(sensor_data.sos);
          Serial.print(F(", Sonne West: "));
          Serial.print(sensor_data.sow);
          Serial.print(F(", Sonne Ost: "));
          Serial.print(sensor_data.soo);
          Serial.print(F(", Daemmerung: "));
          Serial.print(sensor_data.daemmerung);
          Serial.print(F(", Tageslicht: "));
          Serial.print(sensor_data.tageslicht);
          Serial.print(F(", Wind: "));
          Serial.print(sensor_data.wind);
          Serial.print(F(", Regen: "));
          Serial.print(sensor_data.regen);
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

