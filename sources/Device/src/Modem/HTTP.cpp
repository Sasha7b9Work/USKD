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


namespace Updater
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
            NEED_SAPBR_1_1,
            COMPLETED                   // В этом состоянии находися, если обновление завершено или не требуется
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
}


void Updater::Update(pchar answer)
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
                SetState(State::NEED_SAPBR_1_1);
                SIM868::Transmit::With0D("AT+SAPBR=3,1,\"PWD\",\"\"");
            }
        }
        break;

    case State::NEED_SAPBR_1_1:

    case State::COMPLETED:
        break;
    }
}


void Updater::SendMeasuremets(float[TypeMeasure::Count])
{

}
