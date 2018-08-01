#include "Arduino_Elsner-Eltako-Weather-Sensor-RS485.h"


// static const unsigned int kCoverProtection1ActivationLimitKmh = 35;
static const unsigned int kCoverProtection1ActivationLimitKmh = 15;
static const unsigned int kCoverProtection1DeactivationLimitKmh = kCoverProtection1ActivationLimitKmh - 5;
static const unsigned int kCoverProtection1DeactivationMinimumDelayMinutes = 10;
static const uint8_t kCoverProtection1Pin = 6;

// static const unsigned int kCoverProtection2ActivationLimitKmh = 25;
// static const unsigned int kCoverProtection2DeactivationLimitKmh = kCoverProtection2ActivationLimitKmh - 5;
static const unsigned int kCoverProtection2ActivationLimitKmh = 5;
static const unsigned int kCoverProtection2DeactivationLimitKmh = kCoverProtection2ActivationLimitKmh - 3;
static const unsigned int kCoverProtection2DeactivationMinimumMinutes = 10;
static const uint8_t kCoverProtection2Pin = 7;

// proper rounding
#define kmhToTenthMs(kmh)       ((kmh * 100 + 18) / 36)
#define tenthMsToKmh(tenthMs)   ((tenthMs * 36 + 50) / 100)

CoverProtectionData cover_protection_data_1 = {.kNumberForDisplay = 1,
                                               .kActivationLimitTenthMs = kmhToTenthMs(kCoverProtection1ActivationLimitKmh),
                                               .kDeactivationLimitTenthMs = kmhToTenthMs(kCoverProtection1DeactivationLimitKmh),
                                               .kDeactivationMinimumIdleTimeMillis = kCoverProtection1DeactivationMinimumDelayMinutes * 60 * 1000UL,
                                               .kPin = kCoverProtection1Pin,
                                               .is_active = false,
                                               .idle_time_start_millis = 0};
CoverProtectionData cover_protection_data_2 = {.kNumberForDisplay = 2,
                                               .kActivationLimitTenthMs = kmhToTenthMs(kCoverProtection2ActivationLimitKmh),
                                               .kDeactivationLimitTenthMs = kmhToTenthMs(kCoverProtection2DeactivationLimitKmh),
                                               .kDeactivationMinimumIdleTimeMillis = kCoverProtection2DeactivationMinimumMinutes * 60 * 1000UL,
                                               .kPin = kCoverProtection2Pin,
                                               .is_active = false,
                                               .idle_time_start_millis = 0};


void dump_wind_speed(unsigned int tenthMs)
{
    Serial.print(tenthMsToKmh(tenthMs));
    Serial.print(F(" km/h ("));
    Serial.print(tenthMs);
    Serial.print(" m/s*10)");
}

void dump_cover_protection(CoverProtectionData *cover_protection_data)
{
    Serial.print(F("Cover Protection "));
    Serial.print(cover_protection_data->kNumberForDisplay);
    Serial.print(": ");
    dump_wind_speed(cover_protection_data->kActivationLimitTenthMs);
    Serial.print(", ");
    dump_wind_speed(cover_protection_data->kDeactivationLimitTenthMs);
    Serial.print(", ");
    Serial.print(cover_protection_data->kDeactivationMinimumIdleTimeMillis);
    Serial.print(F(" ms, relay pin "));
    Serial.println(cover_protection_data->kPin);

    digitalWrite(cover_protection_data->kPin, HIGH);
    pinMode(cover_protection_data->kPin, OUTPUT);
    digitalWrite(cover_protection_data->kPin, LOW);
    delay(500);
    digitalWrite(cover_protection_data->kPin, HIGH);

    delay(1000);
}


void setup_cover_protection()
{
    dump_cover_protection(&cover_protection_data_1);
    dump_cover_protection(&cover_protection_data_2);
    Serial.println();
}


void check_cover_protection(CoverProtectionData *cover_protection_data)
{
    if (cover_protection_data->is_active)
    {
        // already active, check if we can deactivate
        if (sensor_data.wind_speed_gusts_tenth_ms < cover_protection_data->kDeactivationLimitTenthMs)
        {
            // less wind, so check for minimum idle time
            if (millis() - cover_protection_data->idle_time_start_millis > cover_protection_data->kDeactivationMinimumIdleTimeMillis)
            {
                // enough time with less wind, so we can really deactivate
                cover_protection_data->is_active = false;

                Serial.print(F("Deactivated cover protection "));
                Serial.print(cover_protection_data->kNumberForDisplay);
                Serial.println(".");
            }
        }
        else
        {
            // still windy, so update idle time start timestamp to further postpone deactivation
            cover_protection_data->idle_time_start_millis = millis();
        }
    }
    else if (sensor_data.wind_speed_gusts_tenth_ms >= cover_protection_data->kActivationLimitTenthMs)
    {
        // gusts above upper limit, so activate and set idle time start timestamp
        cover_protection_data->is_active = true;
        cover_protection_data->idle_time_start_millis = millis();

        Serial.print(F("Activated cover protection "));
        Serial.print(cover_protection_data->kNumberForDisplay);
        Serial.print(F(", wind speed gusts is "));
        dump_wind_speed(sensor_data.wind_speed_gusts_tenth_ms);
        Serial.println(".");
    }
}

void loop_cover_protection()
{
    check_cover_protection(&cover_protection_data_1);
    check_cover_protection(&cover_protection_data_2);
}
