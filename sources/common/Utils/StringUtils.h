// 2023/08/23 10:49:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace SU
{
    char *GetWord(pchar string, int pos_start, int pos_end, char buffer[32]);

    // Найти num-ое вхождение подстроки sub_string в строку string
    // num начинается с нуля
    pchar FindSubString(pchar string, pchar sub_string, int num);

    // Возвращает указатель на первый символ за подстрокой sub_string, паче такая будет обнаружена
    pchar AfterSubString(pchar string, pchar sub_string, int num);
}

