// 2023/05/26 12:09:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Modem/MQTT/Sender/StateContactors.h"
#include "Modem/MQTT/MQTT.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include <cstdio>


namespace Sender
{
    namespace StateContactors
    {
        static int state[NUM_PINS_MX] =              // Состояние каждого контактора
        {
         // 1  2  3  4  5  6  7  8  9
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0
        };
        static bool need[NUM_PINS_MX] =    // true, если нужно передавать состояние конактора
        {
            true, true, true, true, true, true, true, true, true,
            true, true, true, true, true, true, true, true, true,
            true, true, true, true, true, true, true, true, true,
            true
        };

        bool AllIsOK(Phase::E phase)
        {
            if (!gset.IsControllingRelays())
            {
                return true;
            }

            int first = phase * 9;

            for (int i = 0; i < 9; i++)
            {
                if (gset.GetNumberSteps() == 4 && i == 3)
                {
                    continue;
                }

                if (state[i + first] == -1)
                {
                    return false;
                }
            }

            return true;
        }

        void SendState(uint num, int _state)
        {
            if (state[num] != _state)
            {
                need[num] = true;

                state[num] = _state;
            }
        }

        void Send100V(bool _st)
        {
            int _state = _st ? 1 : 0;

            if (state[27] != _state)
            {
                need[27] = true;

                state[27] = _state;
            }
        }

        bool SendToSIM800()
        {
            bool sended = false;

            static const char *const names[NUM_PINS_MX] =
            {
                "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9",
                "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9",
                "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9"
            };

            char buffer[100];

            bool all_is_ok = true;

            for (int i = 0; i < NUM_PINS_MX; i++)
            {
                if (need[i])
                {
                    if (i == 27)
                    {
                        MQTT::Packet::Publish("/base/state/dc100v", state[27] == 0 ? "0" : "1");
                        sended = true;
                    }
                    else
                    {
                        std::sprintf(buffer, "base/cont/KM%s", names[i]);
                        MQTT::Packet::Publish(buffer, state[i]);
                        sended = true;
                    }
                    need[i] = false;
                }

                if (state[i] == -1)
                {
                    all_is_ok = false;
                }
            }
            if (sended)
            {
                static bool prev_all_is_ok = !all_is_ok;

                if (prev_all_is_ok != all_is_ok)
                {
                    MQTT::Packet::Publish("/base/state/state_contactors", all_is_ok ? "1" : "0");

                    prev_all_is_ok = all_is_ok;
                }
            }

            return sended;
        }
    }
}
