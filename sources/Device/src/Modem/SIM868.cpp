// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Modem/SIM868.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Hardware/Bootloader.h"
#include "Modem/MQTT/MQTT.h"
#include "Device.h"
#include <cstring>
#include <cstdio>


using namespace Parser;
using namespace std;


namespace SIM868
{
    struct State
    {
        enum E
        {
            START,
            WAIT_ATE0,
            WAIT_BAUDRADE,
            WAIT_GSMBUSY,
            WAIT_CREG_INIT,
            WAIT_REGISTRATION,
            WAIT_IP_INITIAL,
            WAIT_CSTT,
            WAIT_IP_START,
            WAIT_CIICR,
            WAIT_IP_GPRSACT,
            WAIT_CIFSR,
            WAIT_IP_STATUS,
            WAIT_TCP_CONNECT,
            WAIT_CIPHEAD,
            WAIT_CGNSPWR,           // Ожидание включения навигационной части
            WAIT_CGNSURC,           // Ожидание включения автоматической выдачи URC-сообщений
            RUNNING_MQTT
        };

        static void Set(E);
    };

    static State::E state = State::START;

    static bool need_reset = false;

    static TimeMeterMS state_meter;

    void State::Set(E new_state)
    {
        state_meter.Reset();
        state = new_state;
    }

    static bool ProcessUnsolicited(pchar);

    static TimeMeterMS meterCSQ;

    static char levelSignal[16] = { '0', '\0' };

#define METER_IS_RUNNING(t) MeterIsRunning(t, __FILE__, __LINE__)

    static bool MeterIsRunning(uint time, char *file, int line)
    {
        if (state_meter.ElapsedTime() <= time)
        {
            return true;
        }

        Modem::Reset(file, line);

        return false;
    }
}


bool SIM868::ProcessUnsolicited(pchar answer)
{
    if (answer[0])
    {
//        LOG_WRITE("<<<                                     %s", answer);
    }
    else
    {
        return false;
    }

    BufferParser first_word;

    GetWord(answer, 1, &first_word);

    if (std::strcmp(answer, "ERROR") == 0)
    {
        LOG_ERROR("ERROR");
//        Modem::Reset();
        return true;
    }
    else if (std::strcmp(answer, "CLOSED") == 0)
    {
        LOG_WRITE("                  CLOSED");
        Modem::Reset(__FILE__, __LINE__);
        return true;
    }
    else if (std::strcmp(answer, "SEND FAIL") == 0)
    {
        Modem::Reset(__FILE__, __LINE__);
        return true;
    }
    else if (std::strcmp(first_word.data, "+CSQ") == 0)               // Получили ответ на запрос об уровне сигнала
    {
        BufferParser buffer;
        GetWord(answer, 2, &buffer);
        std::strcpy(levelSignal, buffer.data); //-V512
        return true;
    }
    else if (std::strcmp(first_word.data, "+UGNSINF") == 0)
    {
        int number_commas = NumberSymbols(answer, ',');

        if (number_commas > 4)
        {
            DataGPRS data;

            int position1 = PositionSymbol(answer, ',', 3) + 1;
            int position2 = PositionSymbol(answer, ',', 4);

            if (position2 - position1 > 1)
            {
                data.latitude = SymbolsToFloat(answer, position1, position2);
            }

            position1 = PositionSymbol(answer, ',', 4) + 1;
            position2 = PositionSymbol(answer, ',', 5);

            if (position2 - position1 > 1)
            {
                data.longitude = SymbolsToFloat(answer, position1, position2);
            }

            Storage::SetCoordinates(data);
        }
    }
    else if (std::strcmp(answer, "SEND OK") == 0)
    {
        return true;
    }

    return false;
}


void SIM868::Reset()
{
    state = State::START;
}


void SIM868::Update(pchar answer)
{
    if (ProcessUnsolicited(answer))
    {
        return;
    }

    const uint DEFAULT_TIME = 10000;

    if (need_reset)
    {
        need_reset = false;
        state = State::START;
    }

    BufferParser buffer;

    switch (state)
    {
    case State::START:
        LOG_WRITE("SIM868::IDLE");
        SIM868::Transmit::With0D("ATE0");
        State::Set(State::WAIT_ATE0);
        std::strcpy(levelSignal, "0");
        break;

    case State::WAIT_ATE0:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "OK") == 0)
            {
                State::Set(State::WAIT_BAUDRADE);
                SIM868::Transmit::With0D("AT+IPR=115200");
            }
        }
        break;

    case State::WAIT_BAUDRADE:

        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "RDY") == 0)
            {
                State::Set(State::WAIT_GSMBUSY);
                SIM868::Transmit::With0D("AT+GSMBUSY=1");
            }
        }
        break;

    case State::WAIT_GSMBUSY:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "OK") == 0)
            {
                State::Set(State::WAIT_CREG_INIT);
                SIM868::Transmit::With0D("AT+CREG=1");
            }
        }
        break;

    case State::WAIT_CREG_INIT:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "OK") == 0)
            {
                State::Set(State::WAIT_REGISTRATION);
            }
        }
        break;

    case State::WAIT_REGISTRATION:
        if (METER_IS_RUNNING(60000))
        {
            if (std::strcmp(GetWord(answer, 1, &buffer), "+CREG") == 0)
            {
                int stat = GetWord(answer, 2, &buffer)[0] & 0x0f;

                if (stat == 1 ||    // Registered, home network
                    stat == 5)      // Registered, roaming
                {
                    State::Set(State::WAIT_IP_INITIAL);
                    SIM868::Transmit::With0D("AT+CIPSTATUS");
                }
            }
        }
        break;

    case State::WAIT_IP_INITIAL:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(GetWord(answer, 3, &buffer), "INITIAL") == 0)
            {
                State::Set(State::WAIT_CSTT);
                SIM868::Transmit::With0D("AT+CSTT=\"internet\",\"\",\"\"");
            }
        }
        break;

    case State::WAIT_CSTT:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(GetWord(answer, 1, &buffer), "OK") == 0)
            {
                State::Set(State::WAIT_IP_START);
                SIM868::Transmit::With0D("AT+CIPSTATUS");
            }
        }
        break;

    case State::WAIT_IP_START:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(GetWord(answer, 3, &buffer), "START") == 0)
            {
                State::Set(State::WAIT_CIICR);
                SIM868::Transmit::With0D("AT+CIICR");
            }
        }
        break;

    case State::WAIT_CIICR:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(GetWord(answer, 1, &buffer), "OK") == 0)
            {
                State::Set(State::WAIT_IP_GPRSACT);
                SIM868::Transmit::With0D("AT+CIPSTATUS");
            }
        }
        break;

    case State::WAIT_IP_GPRSACT:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(GetWord(answer, 3, &buffer), "GPRSACT") == 0)
            {
                State::Set(State::WAIT_CIFSR);
                SIM868::Transmit::With0D("AT+CIFSR");
            }
        }
        break;

    case State::WAIT_CIFSR:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(GetWord(answer, 1, &buffer), "OK") != 0)
            {
                // Здесь получаем IP-адрес
                State::Set(State::WAIT_IP_STATUS);
                SIM868::Transmit::With0D("AT+CIPSTATUS");
            }
        }
        break;

    case State::WAIT_IP_STATUS:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(GetWord(answer, 3, &buffer), "STATUS") == 0)
            {
                State::Set(State::WAIT_TCP_CONNECT);
                SIM868::Transmit::With0D("AT+CIPSTART=\"TCP\",\"85.143.167.242\",\"1883\"");
            }
        }
        break;

    case State::WAIT_TCP_CONNECT:
        if (METER_IS_RUNNING(160000))
        {
            if (std::strcmp(GetWord(answer, 1, &buffer), "ALREADY") == 0 ||
                std::strcmp(GetWord(answer, 2, &buffer), "OK") == 0)
            {
                State::Set(State::WAIT_CIPHEAD);
                SIM868::Transmit::With0D("AT+CIPHEAD=1");
            }
            else if (std::strcmp(GetWord(answer, 2, &buffer), "FAIL") == 0)
            {
                Modem::Reset(__FILE__, __LINE__);
            }
        }
        break;

    case State::WAIT_CIPHEAD:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "OK") == 0)
            {
                SIM868::Transmit::With0D("AT+CGNSPWR=1");
#ifdef HOME_VERSION
                State::Set(State::RUNNING_MQTT);
#else
                State::Set(State::WAIT_CGNSPWR);
#endif
            }
        }
        break;

    case State::WAIT_CGNSPWR:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "OK") == 0)
            {
                SIM868::Transmit::With0D("AT+CGNSURC=5");
                State::Set(State::WAIT_CGNSURC);
            }
        }

        break;

    case State::WAIT_CGNSURC:
        if (METER_IS_RUNNING(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "OK") == 0)
            {
                State::Set(State::RUNNING_MQTT);
            }
        }

        break;

    case State::RUNNING_MQTT:

        MQTT::Update(answer);

        if (meterCSQ.ElapsedTime() > 5000)
        {
            meterCSQ.Reset();
            SIM868::Transmit::With0D("AT+CSQ");
        }

        break;
    }
}


bool SIM868::IsRegistered()
{
    return (state >= State::WAIT_IP_INITIAL);
}


pchar SIM868::LevelSignal()
{
    return levelSignal;
}
