// 2023/05/18 13:54:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Updater
{
    // ���������� true, ���� Updater ��������� (��������� ���������� ���� ���������� �� ���������)
    bool IsCompleted();

    void CallbackByteFromFTP(char);

    // ���� true - ��������� ����� �� ftp-�������
    bool InModeReceiveDataFromFTP();
}
