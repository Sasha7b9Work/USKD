// 2023/05/26 11:43:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender/Measure.h"
#include "Hardware/Timer.h"
#include <cstdio>
#include <cstring>


namespace Sender
{
    namespace Measure
    {
        static FullMeasure value;
        static bool need = false;
        static TimeMeterMS meter;

        void Send(const FullMeasure &meas)
        {
            if (!meter.IsFinished())
            {
                return;
            }

            meter.SetResponseTime(60000);

            value = meas;

            need = true;
        }

//        static void SendMeasure(pchar name, float voltage)
//        {
//            MQTT::Packet::Publish(name, (int)(voltage + 0.5f));
//        }

        bool SendToSIM800()
        {
            bool result = need;

            if (need)
            {
                if (value.is_good[0])
                {
                    MQTT::Packet::PublishF("base/state/voltage_a", value.measures[0].voltage);
                    MQTT::Packet::PublishF("base/state/current_a", value.measures[0].current);
                    MQTT::Packet::PublishF("/power_a", value.measures[0].GetPower() / 1e3f);
                }

                if (value.is_good[1])
                {
                    MQTT::Packet::PublishF("base/state/voltage_b", value.measures[1].voltage);
                    MQTT::Packet::PublishF("base/state/current_b", value.measures[1].current);
                    MQTT::Packet::PublishF("/power_b", value.measures[1].GetPower() / 1e3f);
                }

                if (value.is_good[2])
                {
                    MQTT::Packet::PublishF("base/state/voltage_c", value.measures[2].voltage);
                    MQTT::Packet::PublishF("base/state/current_c", value.measures[2].current);
                    MQTT::Packet::PublishF("/power_c", value.measures[2].GetPower() / 1e3f);
                }

                need = false;
            }

            return result;
        }
    }
}
