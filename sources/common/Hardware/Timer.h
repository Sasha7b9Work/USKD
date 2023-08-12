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
    uint ElapsedTime();
    void Wait(uint);

    // ������ "���������" ����� time ms (�������� IsFinished() ����� true)
    void SetResponseTime(uint time);
    // ����� true, ����� ���������� ���������� time �� ������� SetResponseTime
    bool IsFinished() const;
private:
    uint time_reset;
    // ����� ����� ������� ����� ������� true � IsFinished()
    uint time_response;
};
