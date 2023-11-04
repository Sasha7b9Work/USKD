// 2023/05/25 21:51:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/MQTT/RequestMQTT.h"


namespace MQTT
{
    void Reset();

    void Update(pchar);

    bool IsConnected();

    void Send(RequestMQTT *);

    namespace Callback
    {
        void OnConnect(bool);
    }

    namespace GET
    {
        void Time();
    }
}
