// 2023/05/26 08:00:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/MQTT/MQTT.h"
#include "Measurer/Measurer.h"


namespace Sender
{
    namespace Measure
    {
        void Send(const FullMeasure &meas);

        bool SendToSIM800();
    }
}
