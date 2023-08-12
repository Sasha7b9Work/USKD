// 2023/06/22 14:00:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"


static Settings def_set =
{
};


Settings gset = def_set;


int Settings::GetKoeffCurrent()
{
    int address = 0x803F804;

    int result = *((int *)address);

    return (result <= 0) ? 16 : result;
}


int Settings::GetNumberSteps()
{
    int address = 0x803F800;

    int result = *((int *)address);

    return result;
}


bool Settings::OnlyMeasure()
{
    return GetNumberSteps() <= 0;
}


bool Settings::IsControllingRelays()
{
    return pinLevel4.IsHi();
}
