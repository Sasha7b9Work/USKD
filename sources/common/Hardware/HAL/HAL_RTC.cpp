// 2023/08/23 10:37:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/StringUtils.h"
#include <cstdlib>
#include <cstdio>
#include <gd32f30x.h>


extern uint timer_counter;


namespace HAL_RTC
{
    static uint time_counter_set;   // Значение счётчика во время первого вызова HAL_RTC::SetDateTime()
    std::time_t time_unix_set;      // Юниксовое время во время вызова HAL_RTC::SetDateTime()


    namespace Calibrator
    {
        struct StructTime
        {
            StructTime(uint _counter = 0, std::time_t _unix = 0) : counter(_counter), unix(_unix) { }
            uint        counter;       // Время по счётчику миллисекунд
            std::time_t unix;          // Время в секундах по юниксу
        };

        static StructTime time_first;
        static StructTime time_last;

        static float koeff = 1.0f;

        static void Append(std::time_t time_unix)
        {
            if (time_first.counter == 0)
            {
                time_first.counter = timer_counter;
                time_first.unix = time_unix;

                time_counter_set = timer_counter;
                time_unix_set = time_unix;

                return;
            }

            time_last.counter = timer_counter;
            time_last.unix = time_unix;

            float delta_sec_counter = (float)(time_last.counter - time_first.counter) / 1000.0f;
            std::time_t delta_sec_unix = time_last.unix - time_first.unix;

            koeff = (float)delta_sec_counter / (float)delta_sec_unix;

            LOG_WRITE("\r\n++++++++++++++++++++++++++++++++++++++++++++ Time counter %f, unix %f, counter / unix = %f\r\n", delta_sec_counter, (float)delta_sec_unix, delta_sec_counter / (float)delta_sec_unix);
        }

        static float GetKoeff()
        {
            return koeff;
        }
    }

    static bool is_set = false;
}


void HAL_RTC::SetDateTime(pchar string)
{
    // "2023-08-22T09:04:28.8621486+00:00"

    char buffer[32];

    std::tm time_now;

    time_now.tm_sec = std::atoi(SU::GetWord(string, 17, 19, buffer));
    time_now.tm_min = std::atoi(SU::GetWord(string, 14, 16, buffer));
    time_now.tm_hour = std::atoi(SU::GetWord(string, 11, 13, buffer));
    time_now.tm_mday = std::atoi(SU::GetWord(string, 8, 10, buffer));
    time_now.tm_mon = std::atoi(SU::GetWord(string, 5, 7, buffer)) - 1;
    time_now.tm_year = std::atoi(SU::GetWord(string, 0, 4, buffer)) - 1900;
    time_now.tm_isdst = 0;
    time_now.tm_wday = 0;
    time_now.tm_yday = 0;

    is_set = true;

    std::time_t time_unix = std::mktime(&time_now);

    Calibrator::Append(time_unix);
}


String PackedTime::ToString() const
{
    return String("%04u-%02u-%02uT%02u:%02u:%02u.000Z", year + 2000, month, day, hours, minutes, seconds);
}


PackedTime HAL_RTC::GetPackedTime(bool *result)
{
    *result = is_set;

    float delta_sec = (float)(timer_counter - time_counter_set) / 1000.0f;

    float k = Calibrator::GetKoeff();

    std::time_t time_unix_now = time_unix_set + (std::time_t)(delta_sec / k);

    std::tm time_now = *std::localtime(&time_unix_now);

    return PackedTime::FromTime(time_now);
}


uint PackedTime::ToSecs() const
{
    std::tm time =
    {
        (int)seconds,
        (int)minutes,
        (int)hours,
        (int)day,
        (int)(month - 1),
        (int)(year + 2000 - 1900),
        0,
        0,
        0
    };

    return (uint)std::mktime(&time);
}


PackedTime PackedTime::FromSecs(uint _secs)
{
    std::time_t secs = { _secs };

    std::tm time = *std::localtime(&secs);

    return FromTime(time);
}


PackedTime PackedTime::FromTime(const std::tm &time)
{
    return PackedTime((uint)time.tm_hour, (uint)time.tm_min, (uint)time.tm_sec, (uint)time.tm_mday, (uint)(time.tm_mon + 1), (uint)(time.tm_year - 100));
}


bool HAL_RTC::TimeIsExist()
{
    return is_set;
}
