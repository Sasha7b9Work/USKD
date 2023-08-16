// 2023/08/16 07:31:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/HTTP/POST/GitConfig.h"
#include "Hardware/Timer.h"
#include "Modem/HTTP/HTTP.h"
#include "Modem/SIM868.h"
#include "Modem/Parser.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/HTTP/POST/Request.h"
#include <cstring>
#include <cstdio>


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
                NEED_HTTPDATA,                  // Здесь устанавливаем количество засылаемых байт
                NEED_SEND_DATA,                 // А здесь посылаем данные
                NEED_CONFIRM_SEND_DATA,         // Ждём подтверждения приёма дынных
                NEED_CONFIRM_HTTPACTION_1_OK,   // Ждём ОК от HTTPACTION=1
                NEED_CONFIRM_HTTPACTION_1_FULL, // Ждём +HTTPACTION от HTTPACTION=1
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

    case State::NEED_HTTPPARA_URL:      WaitSetSend(answer, "OK", State::NEED_HTTPPARA_CONTENT, "AT+HTTPPARA=\"URL\",\"dev.smartwrap.tech:443\"");

    case State::NEED_HTTPPARA_CONTENT:  WaitSetSend(answer, "OK", State::NEED_HTTPDATA, "AT+HTTPPARA=\"CONTENT\",\"application/json\"");
        break;

    case State::NEED_HTTPDATA:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "OK") == 0)
            {
                char message[64];
                char uid[32];
                std::sprintf(message, "/api/config/get{\"%s\":\"test1\"}", HAL::GetUID(uid));
                Request::Set(message);
                SetState(State::NEED_SEND_DATA);
                std::sprintf(message, "AT+HTTPDATA=%d,10000", Request::Lenght());
                SIM868::Transmit::With0D(message);
            }
        }
        break;

    case State::NEED_SEND_DATA:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "DOWNLOAD") == 0)
            {
                SIM868::Transmit::RAW(Request::Get());
                SetState(State::NEED_CONFIRM_SEND_DATA);
            }
        }
        break;

    case State::NEED_CONFIRM_SEND_DATA:
        WaitSetSend(answer, "OK", State::NEED_CONFIRM_HTTPACTION_1_OK, "AT+HTTPACTION=1");
        break;

    case State::NEED_CONFIRM_HTTPACTION_1_OK:
        WaitSetSend(answer, "OK", State::NEED_CONFIRM_HTTPACTION_1_FULL);
        break;

    case State::NEED_CONFIRM_HTTPACTION_1_FULL:
        if (MeterIsRunning(5000))
        {
            if (answer[0])
            {
                if (std::strcmp(GetWord(answer, 1, buffer), "+HTTPACTION") == 0)
                {
                    int i = 0;
                }
            }
        }
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
