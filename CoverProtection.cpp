#include "Arduino_Elsner-Eltako-Weather-Sensor-RS485.h"


#define COVER_PROT_1_PIN    6
#define COVER_PROT_2_PIN    7

// static const int kCoverProtection1ActivationLimitTenthMs = 10 / 3.6 * 35;
static const int kCoverProtection1ActivationLimitTenthMs = 10 / 3.6 * 15;
static const int kCoverProtection1DeactivationLimitTenthMs = kCoverProtection1ActivationLimitTenthMs - (10 / 3.6 * 5);
static const unsigned long kCoverProtection1DeactivationMinimumDelayMillis = 1000UL * 60 * 10;

// static const int kCoverProtection2ActivationLimitTenthMs = 10 / 3.6 * 25;
static const int kCoverProtection2ActivationLimitTenthMs = 10 / 3.6 * 8;
static const int kCoverProtection2DeactivationLimitTenthMs = kCoverProtection2ActivationLimitTenthMs - (10 / 3.6 * 5);
static const unsigned long kCoverProtection2DeactivationMinimumDelayMillis = 1000UL * 60 * 10;

CoverProtectionData cover_protection_data;


void setup_cover_protection()
{
    Serial.print(F("Cover Protection 1: "));
    Serial.print(kCoverProtection1ActivationLimitTenthMs);
    Serial.print(F(" m/s*10, "));
    Serial.print(kCoverProtection1DeactivationLimitTenthMs);
    Serial.print(F(" m/s*10, "));
    Serial.print(kCoverProtection1DeactivationMinimumDelayMillis);
    Serial.println(F(" ms"));
    
    digitalWrite(COVER_PROT_1_PIN, HIGH);
    pinMode(COVER_PROT_1_PIN, OUTPUT);
    digitalWrite(COVER_PROT_1_PIN, LOW);
    delay(500);
    digitalWrite(COVER_PROT_1_PIN, HIGH);
    delay(1000);

    Serial.print(F("Cover Protection 2: "));
    Serial.print(kCoverProtection2ActivationLimitTenthMs);
    Serial.print(F(" m/s*10, "));
    Serial.print(kCoverProtection2DeactivationLimitTenthMs);
    Serial.print(F(" m/s*10, "));
    Serial.print(kCoverProtection2DeactivationMinimumDelayMillis);
    Serial.println(F(" ms"));

    digitalWrite(COVER_PROT_2_PIN, HIGH);
    pinMode(COVER_PROT_2_PIN, OUTPUT);
    digitalWrite(COVER_PROT_2_PIN, LOW);
    delay(500);
    digitalWrite(COVER_PROT_2_PIN, HIGH);
    delay(1000);

    Serial.println();
}


void loop_cover_protection()
{

    if (cover_protection_data.cover_protection_1_is_active)
    {
        // already active, check if we can deactivate
        if (sensor_data.wind_speed_gusts_tenth_ms < kCoverProtection1DeactivationLimitTenthMs)
        {
            // less wind, so check for minimum delay
            if (millis() - cover_protection_data.cover_protection_1_delay_start_millis > kCoverProtection1DeactivationMinimumDelayMillis)
            {
                // enough time with less wind, so we can really deactivate
                cover_protection_data.cover_protection_1_is_active = false;

                Serial.print(F("Deactivated cover protection 1."));
            }
        }
        else
        {
            // still windy, so update delay start timestamp to postpone deactivation
            cover_protection_data.cover_protection_1_delay_start_millis = millis();
        }
    }
    else if (sensor_data.wind_speed_gusts_tenth_ms >= kCoverProtection1ActivationLimitTenthMs)
    {
        // gusts above upper limit, so activate and set delay start timestamp
        cover_protection_data.cover_protection_1_is_active = true;
        cover_protection_data.cover_protection_1_delay_start_millis = millis();

        Serial.print(F("Activated cover protection 1, wind speed gusts is "));
        Serial.print(sensor_data.wind_speed_gusts_tenth_ms / 10); Serial.print("."); Serial.print(sensor_data.wind_speed_gusts_tenth_ms % 10);
        Serial.println(" m/s.");
    }

    if (cover_protection_data.cover_protection_2_is_active)
    {
        // already active, check if we can deactivate
        if (sensor_data.wind_speed_gusts_tenth_ms < kCoverProtection2DeactivationLimitTenthMs)
        {
            // less wind, so check for minimum delay
            if (millis() - cover_protection_data.cover_protection_2_delay_start_millis > kCoverProtection2DeactivationMinimumDelayMillis)
            {
                // enough time with less wind, so we can really deactivate
                cover_protection_data.cover_protection_2_is_active = false;

                Serial.print(F("Deactivated cover protection 2."));
            }
        }
        else
        {
            // still windy, so update delay start timestamp to postpone deactivation
            cover_protection_data.cover_protection_2_delay_start_millis = millis();
        }
    }
    else if (sensor_data.wind_speed_gusts_tenth_ms >= kCoverProtection2ActivationLimitTenthMs)
    {
        // gusts above upper limit, so activate and set delay start timestamp
        cover_protection_data.cover_protection_2_is_active = true;
        cover_protection_data.cover_protection_2_delay_start_millis = millis();

        Serial.print(F("Activated cover protection 2, wind speed gusts is "));
        Serial.print(sensor_data.wind_speed_gusts_tenth_ms / 10); Serial.print("."); Serial.print(sensor_data.wind_speed_gusts_tenth_ms % 10);
        Serial.println(" m/s.");
    }
}
