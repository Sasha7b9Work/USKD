// 2023/04/05 16:54:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define TIME_MS Timer::TimeMS()


namespace Timer
{
    uint TimeMS();

    void DelayMS(uint ms);
}


struct TimeMeterMS
{
    TimeMeterMS()
    {
        Reset();
    }

    void Reset();

    uint ElapsedTime() const;

    void Wait(uint);

    // Теймер "сработает" через time ms (Значение IsFinished() будет true)
    void SetResponseTime(uint time);

    // Вернёт true, когда закончится промежуток time из функции SetResponseTime
    bool IsFinished() const;

private:

    uint time_reset;

    // После этого времени нужно вернуть true в IsFinished()
    uint time_response;
};
