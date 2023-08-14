// 2023/08/14 11:09:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"


namespace HAL_ADC
{
    static float voltage = 0.0f;
}


void HAL_ADC::Init()
{
    pinBAT.Init();
}


float HAL_ADC::GetVoltage()
{
    return voltage;
}
