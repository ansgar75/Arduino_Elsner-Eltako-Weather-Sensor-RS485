#include "Arduino_Elsner-Eltako-Weather-Sensor-RS485.h"

#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01 };
IPAddress ip(192, 168, 0, 11);

EthernetServer server(80);


void setup_webserver()
{

  Serial.print(F("Eth MAC address: "));
  Serial.print(mac[0], HEX); Serial.print(":");
  Serial.print(mac[1], HEX); Serial.print(":");
  Serial.print(mac[2], HEX); Serial.print(":");
  Serial.print(mac[3], HEX); Serial.print(":");
  Serial.print(mac[4], HEX); Serial.print(":");
  Serial.print(mac[5], HEX); Serial.println();
  Serial.print(F("IP address: "));
  Serial.print(ip[0]); Serial.print(".");
  Serial.print(ip[1]); Serial.print(".");
  Serial.print(ip[2]); Serial.print(".");
  Serial.print(ip[3]); Serial.println();
  Serial.println();

  Ethernet.begin(mac, ip);

  server.begin();
}


void loop_webserver()
{

  EthernetClient client = server.available();
  if (client)
  {

    Serial.println(F("Received a request"));

    boolean currentLineIsBlank = true;  // an http request ends with a blank line
    while (client.connected())
    {
      if (client.available())
      {

        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {

          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: application/json"));
          client.println(F("Connnection: close"));
          client.println();
          client.println("{");

          client.print(F("\"readAgeMs\": "));
          client.print(millis() - sensor_data.timestamp_millis);
          client.println(",");
          client.print(F("\"tempTenthC\": "));
          client.print(sensor_data.temperature_tenth_c);
          client.println(",");
          client.print(F("\"tempC\": "));
          if (sensor_data.temperature_tenth_c < 0)
            client.print('-');
          client.print(abs(sensor_data.temperature_tenth_c) / 10); client.print("."); client.print(abs(sensor_data.temperature_tenth_c) % 10);
          client.println(",");
          client.print(F("\"eastKlx\": "));
          client.print(sensor_data.sun_east_klx);
          client.println(",");
          client.print(F("\"southKlx\": "));
          client.print(sensor_data.sun_south_klx);
          client.println(",");
          client.print(F("\"westKlx\": "));
          client.print(sensor_data.sun_west_klx);
          client.println(",");
          client.print(F("\"daylightLx\": "));
          client.print(sensor_data.daylight_lx);
          client.println(",");
          client.print(F("\"isDark\": "));
          client.print(sensor_data.is_dark);
          client.println(",");
          client.print(F("\"windMs\": "));
          client.print(sensor_data.wind_speed_tenth_ms / 10); client.print("."); client.print(sensor_data.wind_speed_tenth_ms % 10);
          client.println(",");
          client.print(F("\"windGustsMs\": "));
          client.print(sensor_data.wind_speed_gusts_tenth_ms / 10); client.print("."); client.print(sensor_data.wind_speed_gusts_tenth_ms % 10);
          client.println(",");
          client.print(F("\"isRain\": "));
          client.print(sensor_data.is_raining);
          client.println(",");
          client.print(F("\"covProt1IsAct\": "));
          client.print(cover_protection_data_1.is_active);
          client.println(",");
          client.print(F("\"covProt2IsAct\": "));
          client.print(cover_protection_data_2.is_active);
          client.println();

          client.println("}");

          break;
        }

        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }

      }
    }
    
    // give the web browser time to receive the data
    delay(1);
    
    // close the connection:
    client.stop();

  }

}
