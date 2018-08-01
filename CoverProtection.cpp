#include "Arduino_Elsner-Eltako-Weather-Sensor-RS485.h"


static const unsigned int kCoverProtection1ActivationLimitKmh = 25;
static const unsigned int kCoverProtection1DeactivationLimitKmh = kCoverProtection1ActivationLimitKmh - 5;
static const unsigned int kCoverProtection1DeactivationMinimumDelayMinutes = 10;
static const uint8_t kCoverProtection1Pin = 6;

static const unsigned int kCoverProtection2ActivationLimitKmh = 35;
static const unsigned int kCoverProtection2DeactivationLimitKmh = kCoverProtection2ActivationLimitKmh - 5;
static const unsigned int kCoverProtection2DeactivationMinimumMinutes = 10;
static const uint8_t kCoverProtection2Pin = 7;

static const unsigned long kRelayPullTimeMillis = 500;
static const unsigned long kRelayFullPeriodMillis = 1000UL * 30;


// proper rounding
#define kmhToTenthMs(kmh)       ((kmh * 100 + 18) / 36)
#define tenthMsToKmh(tenthMs)   ((tenthMs * 36 + 50) / 100)

CoverProtectionData cover_protection_data_1 = {.kNumberForDisplay = 1,
                                               .kActivationLimitTenthMs = kmhToTenthMs(kCoverProtection1ActivationLimitKmh),
                                               .kDeactivationLimitTenthMs = kmhToTenthMs(kCoverProtection1DeactivationLimitKmh),
                                               .kDeactivationMinimumIdleTimeMillis = kCoverProtection1DeactivationMinimumDelayMinutes * 60 * 1000UL,
                                               .kPin = kCoverProtection1Pin,
                                               .is_active = false, .idle_time_start_millis = 0, .relay_cycle_is_active = false, .relay_is_pulled = false, .relay_pulled_start_millis = 0};
CoverProtectionData cover_protection_data_2 = {.kNumberForDisplay = 2,
                                               .kActivationLimitTenthMs = kmhToTenthMs(kCoverProtection2ActivationLimitKmh),
                                               .kDeactivationLimitTenthMs = kmhToTenthMs(kCoverProtection2DeactivationLimitKmh),
                                               .kDeactivationMinimumIdleTimeMillis = kCoverProtection2DeactivationMinimumMinutes * 60 * 1000UL,
                                               .kPin = kCoverProtection2Pin,
                                               .is_active = false, .idle_time_start_millis = 0, .relay_cycle_is_active = false, .relay_is_pulled = false, .relay_pulled_start_millis = 0};

#define relay_pull(pin)    digitalWrite(pin, LOW);
#define relay_release(pin) digitalWrite(pin, HIGH);

unsigned long currentMillis;


void dump_wind_speed(unsigned int tenthMs)
{
    Serial.print(tenthMsToKmh(tenthMs));
    Serial.print(F(" km/h ("));
    Serial.print(tenthMs);
    Serial.print(" m/s*10)");
}

void dump_cover_protection_and_init_relay(CoverProtectionData *cover_protection_data)
{
    // dump values
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

    // init relay
    // TBD: why do relays still snap (only) on reset after uploading sketch???
    pinMode(cover_protection_data->kPin, OUTPUT);
    relay_pull(cover_protection_data->kPin);
    delay(kRelayPullTimeMillis);
    relay_release(cover_protection_data->kPin);

    delay(1000);
}


void setup_cover_protection()
{
    dump_cover_protection_and_init_relay(&cover_protection_data_1);
    dump_cover_protection_and_init_relay(&cover_protection_data_2);
    Serial.println();
}


void check_environment(CoverProtectionData *cover_protection_data)
{
    if (cover_protection_data->is_active)
    {
        // already active, check if we can deactivate
        if (sensor_data.wind_speed_gusts_tenth_ms < cover_protection_data->kDeactivationLimitTenthMs)
        {
            // less wind, so check for minimum idle time
            if (currentMillis - cover_protection_data->idle_time_start_millis > cover_protection_data->kDeactivationMinimumIdleTimeMillis)
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
            cover_protection_data->idle_time_start_millis = currentMillis;
        }
    }
    else if (sensor_data.wind_speed_gusts_tenth_ms >= cover_protection_data->kActivationLimitTenthMs)
    {
        // gusts above upper limit, so activate and set idle time start timestamp
        cover_protection_data->is_active = true;
        cover_protection_data->idle_time_start_millis = currentMillis;

        Serial.print(F("Activated cover protection "));
        Serial.print(cover_protection_data->kNumberForDisplay);
        Serial.print(F(", wind speed gusts is "));
        dump_wind_speed(sensor_data.wind_speed_gusts_tenth_ms);
        Serial.println(".");
    }
}

void control_relay(CoverProtectionData *cover_protection_data)
{
    if (cover_protection_data->is_active)
    {
        if (!cover_protection_data->relay_cycle_is_active || currentMillis - cover_protection_data->relay_pulled_start_millis > kRelayFullPeriodMillis)
        {
            relay_pull(cover_protection_data->kPin);
            cover_protection_data->relay_cycle_is_active = true;
            cover_protection_data->relay_is_pulled = true;
            cover_protection_data->relay_pulled_start_millis = currentMillis;

            Serial.print(F("Pulled relay for cover protection "));
            Serial.print(cover_protection_data->kNumberForDisplay);
            Serial.println(".");
        }
    }
    else
    {
        cover_protection_data->relay_cycle_is_active = false;
    }

    if (cover_protection_data->relay_is_pulled && currentMillis - cover_protection_data->relay_pulled_start_millis > kRelayPullTimeMillis)
    {
        relay_release(cover_protection_data->kPin);
        cover_protection_data->relay_is_pulled = false;

        Serial.print(F("Released relay for cover protection "));
        Serial.print(cover_protection_data->kNumberForDisplay);
        Serial.println(".");
    }
}

void loop_cover_protection()
{
    currentMillis = millis();

    check_environment(&cover_protection_data_1);
    control_relay(&cover_protection_data_1);

    check_environment(&cover_protection_data_2);
    control_relay(&cover_protection_data_2);
}
