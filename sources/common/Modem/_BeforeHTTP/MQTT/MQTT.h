// 2023/05/25 21:51:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace MQTT
{
    void Reset();

    void Update(pchar);

    bool InStateRunning();

    bool InStateIdle();

    void CallbackOnReceiveData(pchar);

    namespace Packet
    {
        void Publish(pchar topic, pchar message);

        void Publish(pchar topic, int);

        void PublishF(pchar topic, float);
    }
}
