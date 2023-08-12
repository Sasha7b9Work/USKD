// 2023/05/26 09:08:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


namespace Sender
{
    namespace StateContactors
    {
        // Состояния
        // -1 - неисправность
        // 0 - выкл
        // 1 - вкл
        void SendState(uint num, int state);

        void Send100V(bool);

        bool AllIsOK(Phase::E);

        bool SendToSIM800();
    }
}
