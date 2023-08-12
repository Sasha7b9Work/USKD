// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Measurer/Contactors.h"
#include "Memory/Memory.h"
#include "Settings/Settings.h"


void Device::Init()
{
    Log::Init();

    HAL::Init();

    LOG_WRITE("   Device::Init()   ");

    Contactors::Init();

    Display::Init();

    Modem::Init();

//    if (!Memory::Test())
//    {
//        LOG_WRITE("Memory test if fail");
//
//        Memory::Test();
//    }
//    else
//    {
//        LOG_WRITE("Memory test is ok");
//    }
}


void Device::Update()
{
    HAL_FWDGT::Update();

    Measurer::Update();

    Display::Update();

    Contactors::Update(Measurer::Measure5Sec());

    Modem::Update();

    HAL_PINS::Update();

    Contactors::Serviceability::Verify();
}
