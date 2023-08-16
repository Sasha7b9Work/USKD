// 2023/08/16 07:31:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/HTTP/POST/GitConfig.h"
#include "Hardware/Timer.h"
#include "Modem/HTTP/HTTP.h"
#include "Modem/SIM868.h"
#include <cstring>


namespace POST
{
    namespace Config
    {
        struct State
        {
            enum E
            {
                IDLE
            };
        };

        static State::E state = State::IDLE;

        static TimeMeterMS state_meter;

        static void SetState(State::E _state)
        {
            state = _state;
            state_meter.Reset();
        }

        static bool MeterIsRunning(uint time)
        {
            if (state_meter.ElapsedTime() < time)
            {
                return true;
            }

            HTTP::Reset();

            return false;
        }

        // Ожидать ответ need,
        // затем установить состояние state,
        // и послать сообщение msg
        void WaitSetSend(pchar answer, pchar need, State::E state, pchar msg = nullptr, uint time = 10000);
    }
}


void POST::Config::Reset()
{
    state = State::IDLE;
}


void POST::Config::Update(pchar)
{

}


void POST::Config::WaitSetSend(pchar answer, pchar need, State::E _state, pchar msg, uint time)
{
    if (MeterIsRunning(time))
    {
        if (std::strcmp(answer, need) == 0)
        {
            SetState(_state);

            if (msg)
            {
                SIM868::Transmit::With0D(msg);
            }
        }
    }
}
