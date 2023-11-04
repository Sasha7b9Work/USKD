// 2023/11/03 15:08:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/MQTT.h"
#include "Utils/String.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include <cstdio>


void MQTT::GET::Time()
{
    String data("{\"serialNumber\":\"%s\"}", HAL::GetUID());

    String message("{\"request\":\"getTime\",\"data\":%s}", data.c_str());

    String response("devices/box/%s/config/getTime", HAL::GetUID());

    MQTT::Send(new RequestPUBLISH("server/request/config", message, response));
}
