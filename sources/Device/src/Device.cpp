// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Hardware/Button.h"
#include "Hardware/LED.h"
#include "Storage/Storage.h"
#include "Modules/GC777/GC777.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT/MQTT.h"
#include <cstring>
#include <gd32f30x.h>


namespace Device
{
    static TimeMeterMS meter;
}


void Device::Init()
{
    HAL::Init();

    LOG_WRITE("Device::Init()");

    Display::Init();

    Modem::Init();

//    Button::Init();

//    ledRED.Init();
//    ledGREEN.Init();
//    ledYELLOW.Init();
}


void Device::Update()
{
    if (MQTT::IsConnected())
    {
        if (meter.ElapsedTime() > 100)
        {
            meter.Reset();

            MQTT::GET::Time();
        }
    }

    Display::Update();

    Modem::Update();
//
//    Storage::Update();
//
//    Measurements measurements;
//
//    if (Storage::GetMeasurements(measurements))
//    {
//        Modem::SendMeasures(measurements);
//    }
}
