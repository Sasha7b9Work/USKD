// 2023/08/23 10:49:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace SU
{
    char *GetWord(pchar string, int pos_start, int pos_end, char buffer[32]);

    // ����� num-�� ��������� ��������� sub_string � ������ string
    // num ���������� � ����
    pchar FindSubString(pchar string, pchar sub_string, int num);

    // ���������� ��������� �� ������ ������ �� ���������� sub_string, ���� ����� ����� ����������
    pchar AfterSubString(pchar string, pchar sub_string, int num);
}

