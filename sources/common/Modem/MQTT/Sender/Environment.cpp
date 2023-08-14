// 2023/08/14 20:17:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender/Environment.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT/MQTT.h"


namespace Sender
{
    namespace Environment
    {
        static float temperature = 0.0f;
        static float humidity = 0.0f;
        static bool need_send = false;
    }
}


void Sender::Environment::SendMeasuremets(float _temperature, float _humidity)
{
    temperature = _temperature;
    humidity = _humidity;
    need_send = true;
}


bool Sender::Environment::SendToSIM868()
{
    static TimeMeterMS meter;

    if (need_send && (meter.ElapsedTime() > 30000))
    {
        need_send = false;

        meter.Reset();

        MQTT::Packet::Publish("/temperature", (int)temperature);

        MQTT::Packet::Publish("/humidity", (int)humidity);

        return true;
    }

    return false;
}


float Sender::Environment::GetTemperature()
{
    return temperature;
}


float Sender::Environment::GetHumidity()
{
    return humidity;
}
