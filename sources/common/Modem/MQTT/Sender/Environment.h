// 2023/08/14 20:19:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Sender
{
    namespace Environment
    {
        void SendMeasuremets(float temperature, float humidity);

        bool SendToSIM868();

        float GetTemperature();

        float GetHumidity();
    }
}
