// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Loader.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Parser.h"
#include "Modem/Updater.h"
#include "Hardware/Programmer.h"
#include "Modem/ReaderFTP.h"
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
AT+FTPCID=1
AT+FTPSERV=\"%s\"", address
AT+FTPUN=\"%s\"", login
AT+FTPPW=\"%s\"", password
AT+FTPGETPATH=\"%s\"", directory
AT+FTPGETNAME=\"%s\"", firmware
AT+FTPGET=1
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
            NEED_FTPCID,            // Находимся в состоянии обновления
            NEED_FTPSERV,           // Имя сервера
            NEED_FTPUN,             // Имя пользователя
            NEED_FTPPW,             // Пароль
            NEED_FTPGETPATH,        // Папка с файлом

            NEED_SET_NAME_FIRMWARE,     // Установить имя файла с версией
            NEED_REQUEST_CONNECT,       // Запрос на соединение
            NEED_WAIT_CONNECT,          // Ждём ответа на файл версии
            GET_BYTES_VER,              // Получение байтов версии
            GET_BYTES_CRC,              // Получение байтов контрольной суммы
            GET_BYTES_FIRMWARE,         // Получение байтов прошивки

            COMPLETED                   // В этом состоянии находися, если обновление завершено или не требуется
        };
    };

    static State::E state = State::IDLE;

    void Reset()
    {
        state = State::IDLE;
    }

    static pchar address("s92153gg.beget.tech");
    static pchar login("s92153gg_1");
    static pchar password("Qwerty123!");
    static pchar directory("/");

    static pchar file_firmware("Meter.frm");

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

        Loader::Reset();

        return false;
    }

    static int version = 0;
    static uint source_crc = 0;         // Здесь хранится контрольная сумма из файла
    static uint crc = 0;                // Подсчитанная crc
}


bool Updater::IsCompleted()
{
    return state == State::COMPLETED;
}


void Updater::CallbackByteFromFTP(char symbol)
{
    ReaderFTP::AppendByte(symbol);
}


bool Updater::InModeReceiveDataFromFTP()
{
    return state == State::GET_BYTES_VER || state == State::GET_BYTES_CRC || state == State::GET_BYTES_FIRMWARE;
}


void Updater::Update(pchar answer)
{
    const uint DEFAULT_TIME = 10000;

    char buffer[32];

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
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPCID);
                SIM868::Transmit::With0D("AT+SAPBR=1,1");
            }
        }
        break;

    case State::NEED_FTPCID:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPSERV);
                SIM868::Transmit::With0D("AT+FTPCID=1");
            }
        }
        break;

    case State::NEED_FTPSERV:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPUN);
                SIM868::Transmit::Format("AT+FTPSERV=\"%s\"", address);
            }
        }
        break;

    case State::NEED_FTPUN:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPPW);
                SIM868::Transmit::Format("AT+FTPUN=\"%s\"", login);
            }
        }
        break;

    case State::NEED_FTPPW:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPGETPATH);
                SIM868::Transmit::Format("AT+FTPPW=\"%s\"", password);
            }
        }
        break;

    case State::NEED_FTPGETPATH:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_SET_NAME_FIRMWARE);
                SIM868::Transmit::Format("AT+FTPGETPATH=\"%s\"", directory);
            }
        }
        break;

    case State::NEED_SET_NAME_FIRMWARE:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_REQUEST_CONNECT);
                SIM868::Transmit::Format("AT+FTPGETNAME=\"%s\"", file_firmware);
            }
        }
        break;

    case State::NEED_REQUEST_CONNECT:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_WAIT_CONNECT);
                SIM868::Transmit::With0D("AT+FTPGET=1");
            }
        }
        break;

    case State::NEED_WAIT_CONNECT:
        if (MeterIsRunning(75000))
        {
            if (strcmp(Parser::GetWord(answer, 1, buffer), "+FTPGET") == 0)
            {
                if (strcmp(answer, "+FTPGET: 1,1") == 0)
                {
                    SetState(State::GET_BYTES_VER);
                    crc = 0;
                    version = 0;
                    ReaderFTP::ReceiveBytes(4);
                }
                else if(strcmp(answer, "+FTPGET: 1,77") == 0)           // Если нет файла на сервере  (77 == Operate error)
                {
                    state = State::COMPLETED;
                }
            }
        }
        break;

    case State::GET_BYTES_VER:
        if (MeterIsRunning(75000))
        {
            if (ReaderFTP::requested_bytes_received)
            {
                memcpy(&version, ReaderFTP::buffer_data, 4);

                // \todo здесь сверяем нужную версию с уже имеющейся

                SetState(State::GET_BYTES_CRC);
                ReaderFTP::ReceiveBytes(4);
            }
        }
        break;

    case State::GET_BYTES_CRC:
        if (MeterIsRunning(75000))
        {
            if (ReaderFTP::requested_bytes_received)
            {
                memcpy(&source_crc, ReaderFTP::buffer_data, 4);

                Programmer::Prepare(HAL_ROM::ADDR_STORAGE);

                SetState(State::GET_BYTES_FIRMWARE);
                HAL_FWDGT::ToUpgradeMode();
                ReaderFTP::ReceiveBytes(ReaderFTP::SIZE_DATA_BUFFER);
                ReaderFTP::received_FTPGET_1_0 = false;
            }
        }
        break;

    case State::GET_BYTES_FIRMWARE:
        if (MeterIsRunning(75000))
        {
            if (ReaderFTP::received_FTPGET_1_0)
            {
                Programmer::WriteBytes(ReaderFTP::buffer_data, ReaderFTP::pointer_data);

                Programmer::CloseSession();

                int written_bytes = Programmer::WrittenBytes();

                crc = Math::CalculateCRC(HAL_ROM::ADDR_STORAGE, written_bytes);

                if (crc == source_crc)
                {
                    {
                        Programmer::CopyFirmware();

                    } while (source_crc != Math::CalculateCRC(HAL_ROM::ADDR_APPLICATION, written_bytes));

                    state = State::COMPLETED;
                }
                else
                {
                    Loader::Reset();
                }
            }
            else if (ReaderFTP::requested_bytes_received)
            {
                Programmer::WriteBytes(ReaderFTP::buffer_data, ReaderFTP::pointer_data);

                state_meter.Reset();
                ReaderFTP::ReceiveBytes(ReaderFTP::SIZE_DATA_BUFFER);
            }
        }
        break;

    case State::COMPLETED:
        break;
    }
}
