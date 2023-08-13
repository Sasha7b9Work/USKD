// 2023/08/12 18:42:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Sender
{
    namespace GPRS
    {
        void SendCoordinates(float, float);

        bool SendToSIM868();

        float GetAltitude();

        float GetLongitude();
    }
}
