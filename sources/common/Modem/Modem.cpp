// 2023/03/16 16:31:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/SIM868.h"
#include "Hardware/Timer.h"
#include "Modem/Parser.h"
#include "Utils/Buffer.h"
#include "Modem/ProcessorBinaryData.h"
#ifdef DEVICE
#include "Modem/MQTT/MQTT.h"
#endif
#ifdef LOADER
#include "Modem/Updater.h"
#endif
#include <gd32f30x.h>
#include <cstring>
#include <cstdlib>


/*
        5. РАБОТА С GSM МОДЕМОМ
    Устройство имеет GSM/GPRS модем Simcom SIM800C, служащий для передачи данных на сервер по GPRS и удаленного управления контроллером.
    Модем имеет собственный источник питания +4В, управляемый портом PD2 (GSM_PWR), 0 – питание включено, 1 – питание выключено.
    Контроль питания модема осуществляется портом PE2 (GSM_PG). Данный порт является толерантным к 5В, поэтому подключен к цепи +4В без делителя.Порядок включения модема.
    1)  Выход GSM_PWR перевести в 1, тем самым обесточив модем.
    2)  Порт GSM_PG перевести в состояние выход 0. Необходимо убедиться, что конденсаторы фильтра питания модема разряжены и на цепи его питания
        нет остаточного напряжения, могущего привести его в неопределенное состояние или зависание. Резистор 1k в цепи станет нагрузкой для разряда конденсаторов.
    3)  Выждать 1000мс для полного разряда емкостей.
    4)  Порт GSM_PG перевести в состояние вход с подтяжкой к земле (pull-down).
    5)  Выход GSM_PWR перевести в 0, тем самым включив модем.
    6)  Ожидать появления 1 на входе GSM_PG, либо выход по таймауту 100мс, при наступлении таймаута считать, что питание модема неисправно и работать оффлайн.
    7)  Ждать не менее 0,5с
    8)  Выход GSM_PWRKEY (PD0) перевести в 0 (аналог удержания кнопки включения) на 1,25с.
    9)  Если в течение 5с на порту PD1 (GSM_STATUS) лог.0 и не появилась единица, то модем по какой-то причине не включился, повторить всю процедуру сначала.
    10)  С момента появления лог.1 на GSM_STATUS готов к работе UART модема, ожидать регистрации в сети не менее 30с, если за 30с регистрации в сети нет, то
        начать всю процедуру сначала.

        Исходные состояния

        GSM_PWR = 1
        GSM_PWRKEY = 1
*/


using namespace std;


namespace Modem
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_DISCHARGE_CAPS,    // Ожидание разряда конденсатора после обесточивания модема
            WAIT_HI_GSM_PG,         // Ожидаине появления 1 на входе GSM_PG после включения модема
            WAIT_500_MS,            // Ожидать полсекунды после появления 1 на входе GSM_PG
            WAIT_1250_MS,           // Ожидать 1.25 сек после перевода GSM_PWRKEY в 0
            WAIT_5000_MS,           // Ожидать 5 секунд единицу на GSM_STATUS
            HARDWARE_IS_OK,         // Железо отработало, работают команды
        };

        static void Set(E v)
        {
            state = v;
        }

        static E Current()
        {
            return state;
        }

    private:

        static E state;
    };

    State::E State::state = State::IDLE;

    // Данные, получаемые от SIM868
    namespace InData
    {
        static Buffer<2048> main;
        static Buffer<2048> addit;
        static Buffer<2048> buffer;

        void Update()
        {
            main.mutex.Try();

            if (main.Size())
            {
                for (int i = 0; i < main.Size(); i++)
                {
//                    Log::ReceiveFromSIM800(main[i]);
                }
                if (!buffer.Append(main.Data(), main.Size()))
                {
                    buffer.Clear();
                }
                main.Clear();
            }

            main.mutex.Release();

            if (buffer.Size() == 0)
            {
                SIM868::Update("");
            }
            else
            {
                Buffer<2048> answer;

                bool answer_exist = false;

                static int binary_bytes_left = 0;

                do
                {
                    answer.Clear();
                    answer_exist = false;

                    for (int i = 0; i < buffer.Size(); i++)
                    {
                        char symbol = buffer[i];

                        if (binary_bytes_left)
                        {
//                            LOG_WRITE_TRACE("byte %02X", (uint8)symbol);
                            PBD::AppendByte((uint8)symbol);
                            binary_bytes_left--;
                            continue;
                        }

                        if (symbol == 0x0a)
                        {
                            continue;
                        }
                        else if (symbol == 0x0d)
                        {
                            if (answer.Size() == 0)
                            {
                                continue;
                            }
                            else
                            {
                                answer.Append('\0');
                                answer_exist = true;
                                buffer.RemoveFirst(i + 1);
                                break;
                            }
                        }
                        else if (symbol == '>')
                        {
                            answer.Append('>');
                            answer.Append('\0');
                            answer_exist = true;
                            buffer.RemoveFirst(i + 1);
                            break;
                        }
                        else
                        {
                            answer.Append(symbol);

                            if (answer.Size() > 5 && answer[3] == 'D')
                            {
                                if (answer[answer.Size() - 1] == ':')
                                {
                                    if (std::memcmp(answer.Data(), "+IPD", 4) == 0)
                                    {
                                        char *pointer = answer.Data() + 5;

                                        char buf_number[32];
                                        char *p_buffer = buf_number;

                                        while (*pointer != ':')
                                        {
                                            *p_buffer++ = *pointer++;
                                        }

                                        *p_buffer = '\0';

                                        binary_bytes_left = (int)std::strtoul(buf_number, nullptr, 10);

                                        i = -1;
                                        while (buffer[0] < 0x20)
                                        {
                                            buffer.RemoveFirst(1);
                                        }
                                        buffer.RemoveFirst(answer.Size());
                                        answer.Clear();

                                        LOG_WRITE_TRACE("received %d bytes it %d ms", binary_bytes_left, TIME_MS);

                                        continue;
                                    }
                                }
                            }
                        }
                    }

                    SIM868::Update(answer_exist ? answer.Data() : "");

                } while (answer_exist);
            }
        }
    }

    namespace GSM_PG
    {
        static uint PortPG()
        {
#ifdef HOME_VERSION
            return GPIOE;
#else
            return GPIOD;
#endif
        }

        static void ToOutLow();

        static void ToInPullDown();

        static bool ReadInput();
    }
}


void Modem::CallbackOnReceive(char symbol)
{
#ifdef LOADER
    if (Updater::InModeReceiveDataFromFTP())
    {
        Updater::CallbackByteFromFTP(symbol);
        return;
    }
#endif

    if (!InData::main.mutex.IsBusy())
    {
        if (InData::addit.Size())
        {
            if (!InData::main.Append(InData::addit.Data(), InData::addit.Size()))
            {
                InData::main.Clear();
            }
            InData::addit.Clear();
        }

        if (!InData::main.Append(symbol))
        {
            InData::main.Clear();
        }
    }
    else
    {
        if (!InData::addit.Append(symbol))
        {
            InData::addit.Clear();
        }
    }
}


void Modem::Init()
{
    pinGSM_PWR.Init();
    pinGSM_PWR.Set();

    pinGSM_PWRKEY.Init();
    pinGSM_PWRKEY.Reset();

    pinGSM_STATUS.Init();

    pinGSM_STATUS.DeInit();

    HAL_USART_GPRS::Init();

    HAL_USART_GPRS::DeInit();
}


void Modem::Reset(char *file, int line)
{
    (void)file;
    (void)line;

    State::Set(State::IDLE);

    LOG_WRITE("Modem::Reset from %s : %d", file, line);

//    while (true)
//    {
//        // Здесь сработает watchdog       \todo не соединяется с сервером MQTT в этом случае
//    }
}


void Modem::Update()
{
    static TimeMeterMS meter;

    static int prev_state = -1;

    if (State::Current() != prev_state)
    {
        prev_state = State::Current();

//        LOG_WRITE("State modem : %d", prev_state);
    }

    switch (State::Current())
    {
    case State::IDLE:
        LOG_WRITE("Modem::IDLE");
        SIM868::Reset();
#ifdef DEVICE
        MQTT::Reset();
#endif
        pinGSM_PWR.Set();
        GSM_PG::ToOutLow();
        State::Set(State::WAIT_DISCHARGE_CAPS);
        meter.Reset();
        break;

    case State::WAIT_DISCHARGE_CAPS:
        if (meter.ElapsedTime() > 3000)
        {
            GSM_PG::ToInPullDown();
            pinGSM_PWR.Reset();
            State::Set(State::WAIT_HI_GSM_PG);
            meter.Reset();
        }
        break;

    case State::WAIT_HI_GSM_PG:
        if (GSM_PG::ReadInput())
        {
            meter.Reset();
            State::Set(State::WAIT_500_MS);
            pinGSM_STATUS.Init();
            HAL_USART_GPRS::Init();
        }
        if (meter.ElapsedTime() > 100)
        {
            LOG_ERROR("Reset to state idle");
            State::Set(State::IDLE);
        }

        break;

    case State::WAIT_500_MS:
        if (meter.ElapsedTime() > 500)
        {
            pinGSM_PWRKEY.Reset();
            meter.Reset();
            State::Set(State::WAIT_1250_MS);
        }
        break;

    case State::WAIT_1250_MS:
        if (meter.ElapsedTime() > 1250)
        {
            pinGSM_PWRKEY.Set();
            meter.Reset();
            State::Set(State::WAIT_5000_MS);
        }
        break;

    case State::WAIT_5000_MS:
        if (pinGSM_STATUS.IsHi())
        {
            State::Set(State::HARDWARE_IS_OK);
            meter.Reset();
        }
        if (meter.ElapsedTime() > 5000)
        {
            State::Set(State::IDLE);
        }
        break;

    case State::HARDWARE_IS_OK:
        InData::Update();
        break;
    }
}


bool Modem::Mode::Power()
{
    return (State::Current() == State::HARDWARE_IS_OK);
}


bool Modem::ExistUpdate()
{
    return false;
}


void Modem::GSM_PG::ToOutLow()
{
    gpio_init(PortPG(), GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    GPIO_BC(PortPG()) = GPIO_PIN_2;
}


void Modem::GSM_PG::ToInPullDown()
{
    gpio_init(PortPG(), GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
}


bool Modem::GSM_PG::ReadInput()
{
    return gpio_input_bit_get(PortPG(), GPIO_PIN_2) == SET;
}


#ifdef DEVICE
void Modem::SendMeasures(const Measurements &/*meas*/)
{
//    Swagger::SendMeasuremets(meas);
}
#endif
