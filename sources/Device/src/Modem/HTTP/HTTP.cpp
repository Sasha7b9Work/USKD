// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Parser.h"
#include "Modem/HTTP/HTTP.h"
#include "Modem/SIM868.h"
#include "Utils/Math.h"
#include "Modem/HTTP/POST/GitConfig.h"
#include <gd32f30x.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>


/*
*   *** FTP-сервер ***
*   s92153gg.beget.tech/files
*   s92153gg_1
*   Qwerty123!
*/


/*
AT+CREG?
AT+CIPSTATUS
+INITIAL
AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"
AT+SAPBR=3,1,\"APN\",\"internet\"
AT+SAPBR=3,1,\"USER\",\"\"
AT+SAPBR=3,1,\"PWD\",\"\"
AT+SAPBR=1,1
*/


using namespace Parser;
using namespace std;


namespace HTTP
{
    struct State
    {
        enum E
        {
            IDLE,
            NEED_SAPBR_3_GPRS,
            NEED_SAPBR_3_APN,
            NEED_SAPBR_3_USER,
            NEED_SAPBR_3_PWD,
            WAIT_PASSWORD_OK,
            POST_GET_CONFIG             // Находимся в состоянии получения конфига
//            WAIT_COMMANDS,              // Ожидаем команды для передачи
//            NEED_SEND,                  // Нуждаемся в передаче команды
//            NEED_SAPBR_2_1,
//            NEED_HTTPINIT,
//            NEED_HTTPPARA_CID,
//            NEED_HTTPPARA_URL,
//            NEED_HTTPPARA_CONTENT,
//            NEED_HTTPDATA,
//            NEED_SEND_DATA,
//            NEED_HTTPACTION_1,
//            NEED_HTTPREAD,
//            NEED_TTTPTERM,
//            NEED_SAPBR_0_1
        };
    };

    static State::E state = State::IDLE;

    void Reset()
    {
        state = State::IDLE;
    }

    void Update(const pchar);

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

        Reset();
        Modem::Reset();

        return false;
    }

    static float measurements[TypeMeasure::Count];

    // Ожидать ответ need,
    // затем установить состояние state,
    // и послать сообщение msg
    void WaitSetSend(pchar answer, pchar need, State::E state, pchar msg = nullptr, uint time = 10000);
}


void HTTP::Update(pchar answer)
{
    switch (state)
    {
    case State::IDLE:
        state = State::NEED_SAPBR_3_GPRS;
        break;

    case State::NEED_SAPBR_3_GPRS:
        SetState(State::NEED_SAPBR_3_APN);
        SIM868::Transmit::With0D("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
        break;

    case State::NEED_SAPBR_3_APN:
        WaitSetSend(answer, "OK", State::NEED_SAPBR_3_USER, "AT+SAPBR=3,1,\"APN\",\"internet\"", 85000);
        break;

    case State::NEED_SAPBR_3_USER:
        WaitSetSend(answer, "OK", State::NEED_SAPBR_3_PWD, "AT+SAPBR=3,1,\"USER\",\"\"", 85000);
        break;

    case State::NEED_SAPBR_3_PWD:
        WaitSetSend(answer, "OK", State::WAIT_PASSWORD_OK, "AT+SAPBR=3,1,\"PWD\",\"\"", 85000);
        break;

    case State::WAIT_PASSWORD_OK:
        WaitSetSend(answer, "OK", State::POST_GET_CONFIG);
        break;

    case State::POST_GET_CONFIG:
        POST::Config::Update(answer);
        break;
    }
}


void HTTP::SendMeasuremets(float meas[TypeMeasure::Count])
{
    for (int i = 0; i < TypeMeasure::Count; i++)
    {
        measurements[i] = meas[i];
    }

//    if (state == State::WAIT_COMMANDS)
//    {
//        SetState(State::NEED_SEND);
//    }
}


void HTTP::WaitSetSend(pchar answer, pchar need, State::E _state, pchar msg, uint time)
{
    if (MeterIsRunning(time))
    {
        if (strcmp(answer, need) == 0)
        {
            SetState(_state);

            if (msg)
            {
                SIM868::Transmit::With0D(msg);
            }
        }
    }
}
