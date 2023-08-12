// 2023/05/29 15:29:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/MQTT/Sender/Measure.h"
#include "Modem/MQTT/Sender/LevelContactors.h"
#include "Modem/MQTT/Sender/StateContactors.h"
#include "Modem/MQTT/Sender/StringState.h"
#include "Modem/MQTT/Sender/GP.h"
#include "Modem/SIM800.h"


namespace Sender
{
    void Reset();

    bool SendToSIM800();

    bool SendAll(pchar);

    void ResetMeter();
}
