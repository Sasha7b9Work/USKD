// 2023/08/24 21:29:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct LED
{
    LED(uint _port, uint _pin) : port(_port), pin(_pin) { };
    void Init();
    void On();
    void Off();
private:
    uint port;
    uint pin;
};


extern LED ledYELLOW;
extern LED ledGREEN;
extern LED ledRED;
