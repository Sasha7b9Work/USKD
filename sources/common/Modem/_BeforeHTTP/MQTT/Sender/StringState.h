// 2023/06/01 13:47:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Sender
{
    namespace StringState
    {
        // ���� now == true, ������� ����� ������
        void Send(pchar, bool now);

        bool SendToSIM800();

        void Reset();
    }
}
