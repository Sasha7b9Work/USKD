// 2023/05/26 12:14:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender/GP.h"
#include "Modem/MQTT/MQTT.h"


namespace Sender
{
    namespace GP
    {
        static bool value[3] = { false, false, false };
        static bool need[3] = { true, true, true };

        void Send(int num, bool is_low)
        {
            if (!MQTT::InStateRunning())
            {
                return;
            }

            value[num - 1] = is_low;

            need[num - 1] = true;
        }

        bool SendToSIM800()
        {
            bool need_send = need[0] || need[1] || need[2];

            if (need_send)
            {
                char name[20] = "/base/state/gp0";

                for (int i = 0; i < 3; i++)
                {
                    if (need[i])
                    {
                        name[14] = (char)((i + 1) | 0x30);

                        MQTT::Packet::Publish(name, value[i] ? "1" : "0");

                        need[i] = false;
                    }
                }
            }

            return need_send;
        }
    }
}
