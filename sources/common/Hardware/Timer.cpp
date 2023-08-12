// 2023/04/05 16:54:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/systick.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


uint timer_counter = 0;


uint Timer::TimeMS()
{
    return timer_counter;
}


void TimeMeterMS::Reset()
{
    time_reset = timer_counter;
    time_response = timer_counter;
}


uint TimeMeterMS::ElapsedTime()
{
    return timer_counter - time_reset;
}


void TimeMeterMS::Wait(uint time)
{
    uint end_time = ElapsedTime() + time;

    while (ElapsedTime() < end_time)
    {
        Modem::Update();
    }
}


void TimeMeterMS::SetResponseTime(uint time)
{
    time_response = timer_counter + time;
}


bool TimeMeterMS::IsFinished() const
{
    return (timer_counter >= time_response);
}


void Timer::DelayMS(uint ms)
{
    uint time_end = TimeMS() + ms;

    while (TimeMS() < time_end)
    {
    }
}
