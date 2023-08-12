// 2023/05/26 09:08:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


namespace Sender
{
    namespace StateContactors
    {
        // ���������
        // -1 - �������������
        // 0 - ����
        // 1 - ���
        void SendState(uint num, int state);

        void Send100V(bool);

        bool AllIsOK(Phase::E);

        bool SendToSIM800();
    }
}
