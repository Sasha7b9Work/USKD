// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Hardware/DHT22/DHT22.h"
#include "Storage.h"


void Device::Init()
{
    Log::Init();

    HAL::Init();

    Display::Init();

    Modem::Init();
}


void Device::Update()
{
    HAL_FWDGT::Update();

    DHT::Update();

    Display::Update();

    Modem::Update();

    Storage::Update();
}
