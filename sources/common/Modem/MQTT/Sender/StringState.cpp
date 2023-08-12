// 2023/06/01 13:48:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender/Sender.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"


namespace Sender
{
    namespace StringState
    {
        static pchar state = "Running";
        static bool need_send = true;
    }
}


void Sender::StringState::Reset()
{
    need_send = true;
}


void Sender::StringState::Send(pchar _state, bool now)
{
    state = _state;

    if (now)
    {
        Sender::ResetMeter();

        TimeMeterMS meter;

        while (need_send && meter.ElapsedTime() < 10000)
        {
            Modem::Update();
        }
    }
}


bool Sender::StringState::SendToSIM800()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() > 1000)
    {
        if (need_send)
        {
            need_send = false;

            MQTT::Packet::Publish("/state", state);

            return true;
        }
    }

    return false;
}
