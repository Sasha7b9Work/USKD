// 2023/08/16 07:31:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/HTTP/POST/GitConfig.h"
#include "Hardware/Timer.h"
#include "Modem/HTTP/HTTP.h"
#include "Modem/SIM868.h"
#include "Modem/Parser.h"
#include <cstring>


using namespace Parser;


namespace POST
{
    namespace Config
    {
        struct State
        {
            enum E
            {
                IDLE,
                NEED_SAPBR_2_1,
                NEED_HTTPINIT,
                NEED_HTTPPARA_CID_1,
                NEED_HTTPPARA_URL,
                NEED_HTTPPARA_CONTENT,
//            NEED_HTTPDATA,
//            NEED_SEND_DATA,
//            NEED_HTTPACTION_1,
//            NEED_HTTPREAD,
//            NEED_TTTPTERM,
//            NEED_SAPBR_0_1

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

        static const uint DEFAULT_TIME = 10000;

        // Ожидать ответ need,
        // затем установить состояние state,
        // и послать сообщение msg
        void WaitSetSend(pchar answer, pchar need, State::E state, pchar msg = nullptr, uint time = DEFAULT_TIME);
    }
}


void POST::Config::Reset()
{
    state = State::IDLE;
}


void POST::Config::Update(pchar answer)
{
    char buffer[64];

    switch (state)
    {
    case State::IDLE:           SetState(State::NEED_SAPBR_2_1);
                                SIM868::Transmit::With0D("AT+SAPBR=1,1");
        break;

    case State::NEED_SAPBR_2_1: WaitSetSend(answer, "OK", State::NEED_HTTPINIT, "AT+SAPBR=2,1");
        break;

    case State::NEED_HTTPINIT:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (std::strcmp(GetWord(answer, 1, buffer), "+SAPBR") == 0)
            {
                int first = GetWord(answer, 2, buffer)[0] & 0x0f;
                int second = GetWord(answer, 3, buffer)[0] & 0x0f;

                if (first == 1 && second == 1)
                {
                    SetState(State::NEED_HTTPPARA_CID_1);
                    SIM868::Transmit::With0D("AT+HTTPINIT");
                }
            }
        }
        break;

    case State::NEED_HTTPPARA_CID_1:    WaitSetSend(answer, "OK", State::NEED_HTTPPARA_URL, "AT+HTTPPARA=\"CID\",1");
        break;

    case State::NEED_HTTPPARA_URL:      WaitSetSend(answer, "OK", State::NEED_HTTPPARA_CONTENT, "AT+HTTPPARA=\"URL\",\"https://dev.smartwrap.tech/:443\"");

    case State::NEED_HTTPPARA_CONTENT:
        break;
    }
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
