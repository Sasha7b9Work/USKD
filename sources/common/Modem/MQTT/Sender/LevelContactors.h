// 2023/05/26 08:22:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


namespace Sender
{
    namespace LevelContactors
    {
        void Send(const int level[Phase::Count]);

        bool SendToSIM800();
    }
}
