// 2023/06/22 13:59:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct Settings
{
    int GetKoeffCurrent();          // ������������ �������������� ����

    int GetNumberSteps();           // ����� �������� �������������. ���� -1, �� ������������� �� ������������

    bool IsControllingRelays();     // ���� true, �� �������������� ��������� ���� � ��������� ����, ���� ���� �� ���� ���� �� ��� ����������

    bool OnlyMeasure();             // ���� true, �� ������������ ������ ���������, ��� �������������
};


extern Settings gset;
