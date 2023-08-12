// 2023/08/12 18:43:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender/GPRS.h"
#include "Modem/MQTT/MQTT.h"


namespace Sender
{
    namespace GPRS
    {
        static float altitude = 0.0f;
        static float longitude = 0.0f;
        static bool need_send = false;
    }
}


void Sender::GPRS::SendCoordinates(float _alt, float _long)
{
    altitude = _alt;
    longitude = _long;
    need_send = true;
}


bool Sender::GPRS::SendToSIM868()
{
    if (need_send)
    {
        need_send = false;

        MQTT::Packet::Publish("/altitude", (int)altitude);
        MQTT::Packet::Publish("/longitude", (int)longitude);

        return true;
    }

    return false;
}
