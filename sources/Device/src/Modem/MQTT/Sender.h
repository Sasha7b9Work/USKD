// 2023/10/22 18:15:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/SIM868.h"


namespace Sender
{
    void Reset();

    bool SendAll(pchar);

    void SendTemperature(float);

    void SendHumidity(float);

    void SendCharge(float);

    void SendLatitude(float);

    void SendDataGPRS(const DataGPRS &);
}
