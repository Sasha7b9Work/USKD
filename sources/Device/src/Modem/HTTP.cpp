// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Parser.h"
#include "Modem/HTTP.h"
#include "Modem/SIM868.h"
#include "Utils/Math.h"
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
            WAIT_COMMANDS,              // Ожидаем команды для передачи
            NEED_SEND,                  // Нуждаемся в передаче команды
            NEED_SAPBR_2_1,
            NEED_HTTPINIT,
            NEED_HTTPPARA_CID,
            NEED_HTTPPARA_URL,
            NEED_HTTPPARA_CONTENT,
            NEED_HTTPDATA,
            NEED_SEND_DATA,
            NEED_HTTPACTION_1,
            NEED_HTTPREAD,
            NEED_TTTPTERM,
            NEED_SAPBR_0_1
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

        Modem::Reset();

        return false;
    }

    static float measurements[TypeMeasure::Count];
}


void HTTP::Update(pchar answer)
{
    const uint DEFAULT_TIME = 10000;

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
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_SAPBR_3_USER);
                SIM868::Transmit::With0D("AT+SAPBR=3,1,\"APN\",\"internet\"");
            }
        }
        break;

    case State::NEED_SAPBR_3_USER:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_SAPBR_3_PWD);
                SIM868::Transmit::With0D("AT+SAPBR=3,1,\"USER\",\"\"");
            }
        }
        break;

    case State::NEED_SAPBR_3_PWD:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::WAIT_PASSWORD_OK);
                SIM868::Transmit::With0D("AT+SAPBR=3,1,\"PWD\",\"\"");
            }
        }
        break;

    case State::WAIT_PASSWORD_OK:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::WAIT_COMMANDS);
            }
        }
        break;

    case State::WAIT_COMMANDS:
        break;

    case State::NEED_SEND:
        SetState(State::NEED_SAPBR_2_1);
        SIM868::Transmit::With0D("AT+SAPBR=1,1");
        break;

    case State::NEED_SAPBR_2_1:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_HTTPINIT);
                SIM868::Transmit::With0D("AT+SAPBR=2,1");
            }
        }
        break;

    case State::NEED_HTTPINIT:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_HTTPPARA_CID);
                SIM868::Transmit::With0D("AT + HTTPINIT");
            }
        }
        break;

    case State::NEED_HTTPPARA_CID:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_HTTPPARA_URL);
                SIM868::Transmit::With0D("AT+HTTPPARA=CID,1");
            }
        }
        break;

    case State::NEED_HTTPPARA_URL:
        break;

    case State::NEED_HTTPPARA_CONTENT:
        break;

    case State::NEED_HTTPDATA:
        break;

    case State::NEED_SEND_DATA:
        break;

    case State::NEED_HTTPACTION_1:
        break;

    case State::NEED_HTTPREAD:
        break;

    case State::NEED_TTTPTERM:
        break;

    case State::NEED_SAPBR_0_1:
        break;
    }
}


void HTTP::SendMeasuremets(float meas[TypeMeasure::Count])
{
    for (int i = 0; i < TypeMeasure::Count; i++)
    {
        measurements[i] = meas[i];
    }

    if (state == State::WAIT_COMMANDS)
    {
        SetState(State::NEED_SEND);
    }
}
