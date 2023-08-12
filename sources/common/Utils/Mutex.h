// 2023/05/26 13:51:50 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


class Mutex
{
public:
    Mutex() : busy(false) { }
    void Try()
    {
        busy = true;
    }
    void Release()
    {
        busy = false;
    }
    bool IsBusy() const
    {
        return busy;
    }
private:

    bool busy;
};
