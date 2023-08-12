// 2023/05/29 15:29:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/MQTT/Sender/StringState.h"
#include "Modem/SIM868.h"


namespace Sender
{
    void Reset();

    bool SendToSIM800();

    bool SendAll(pchar);

    void ResetMeter();
}
