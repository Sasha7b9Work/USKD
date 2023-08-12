// 2023/05/29 15:29:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender/Sender.h"
#include "Modem/MQTT/Sender/Counter.h"
#include "Modem/MQTT/MQTT.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include <cstdio>
#include <cstring>


namespace Sender
{
    static bool versionSW_is_sended = false;

    static TimeMeterMS meter;

    static void SendLastReset();

    // По этому таймеру будем посылать пинги
    static TimeMeterMS meterPing;

    // Если true - нужно посылать пинг
    static bool need_ping = false;
}


void Sender::Reset()
{
    Counter::Reset();

    StringState::Reset();

    versionSW_is_sended = false;

    meter.SetResponseTime(0);
}


bool Sender::SendToSIM800()
{
    static TimeMeterMS local_meter;

    if (local_meter.ElapsedTime() > 1000)
    {
        if (!versionSW_is_sended)
        {
            versionSW_is_sended = true;

            char buffer[32];

            std::sprintf(buffer, "v%d:%d:%d", VERSION, gset.GetNumberSteps(), gset.GetKoeffCurrent());

            MQTT::Packet::Publish("/versionSW", buffer);
            versionSW_is_sended = true;

            MQTT::Packet::Publish("base/id", HAL::GetUID(buffer));

            SendLastReset();

            return true;
        }
    }

    return false;
}


void Sender::ResetMeter()
{
    meter.SetResponseTime(0);
}


bool Sender::SendAll(pchar answer)
{
    if (meterPing.ElapsedTime() > 20000)
    {
        meterPing.Reset();
        need_ping = true;
    }

    if (std::strcmp(answer, ">") == 0)
    {
        bool sending = false;

        if (need_ping)
        {
            sending = true;
            need_ping = false;
            SIM800::Transmit::UINT8(0xC0);
            SIM800::Transmit::UINT8(0x00);
        }
        else if (Sender::SendToSIM800())
        {
            sending = true;
        }
        else if (Sender::StringState::SendToSIM800())
        {
            sending = true;
        }
        else if (Sender::Measure::SendToSIM800())
        {
            sending = true;
        }
        else if (Sender::LevelContactors::SendToSIM800())
        {
            sending = true;
        }
        else if (Sender::StateContactors::SendToSIM800())
        {
            sending = true;
        }
        else if (Sender::GP::SendToSIM800())
        {
            sending = true;
        }
        else if (Sender::Counter::SendToSIM800())
        {
            sending = true;
        }

        SIM800::Transmit::UINT8(sending ? (uint8)0x1A : (uint8)0x1B);

        if (sending)
        {
            SIM800::Transmit::With0D("AT+CIPSEND");
        }

        return true;
    }
    else
    {
        if (meter.IsFinished())
        {
            SIM800::Transmit::With0D("AT+CIPSEND");

            meter.SetResponseTime(1000);
        }
    }

    return false;
}


void Sender::SendLastReset()
{
    MQTT::Packet::Publish("/last/reset", "-");

    if (_GET_BIT(GL::_RCU_RSTSCK, 28))
    {
        MQTT::Packet::Publish("/last/reset", "Software");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 31))
    {
        MQTT::Packet::Publish("/last/reset", "Low power");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 30))
    {
        MQTT::Packet::Publish("/last/reset", "Watchdog");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 29))
    {
        MQTT::Packet::Publish("/last/reset", "Free watchdog");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 27))
    {
        MQTT::Packet::Publish("/last/reset", "Power");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 26))
    {
        MQTT::Packet::Publish("/last/reset", "External pin");
    }
}
