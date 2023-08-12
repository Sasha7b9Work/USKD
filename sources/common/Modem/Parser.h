// 2023/4/30 18:04:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Parser
{
    // Возвращает num-ое слово. Разделителями являются ':', ' ', ','. Если слово в кавычках, то кавычки опускаются
    // Нумерация начинается с 1
    pchar GetWord(pchar, int num, char out[32]);
    pchar GetWordInQuotes(pchar, int num, char out[32]);

    int NumberSymbols(pchar, char);

    // num начинается с единицы
    int PositionSymbol(pchar, char symbol, int num);
    int PositionSymbol(pchar, char symbol, int num);

    // Копирует в out символы, расположенные между позициями start и end
    pchar GetWord(pchar, int start, int end, char out[32]);
}
