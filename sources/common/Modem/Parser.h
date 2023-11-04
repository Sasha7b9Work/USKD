// 2023/4/30 18:04:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

#ifdef DEVICE
    #include "Storage/Storage.h"
#endif


struct BufferParser
{
    char data[128];

    uint ToUINT() const;
};


namespace Parser
{
    // Возвращает num-ое слово. Разделителями являются ':', ' ', ','. Если слово в кавычках, то кавычки опускаются
    // Нумерация начинается с 1
    pchar GetWord(pchar, int num, BufferParser *);
    pchar GetWordInQuotes(pchar, int num, BufferParser *);

    int NumberSymbols(pchar, char);

    // num начинается с единицы
    int PositionSymbol(pchar, char symbol, int num);

    // Копирует в out символы, расположенные между позициями start и end
    pchar GetWord(pchar, int start, int end, BufferParser *);

    float SymbolsToFloat(pchar, int pos_start, int pos_end);

#ifdef DEVICE
    pchar ParseConfigField(pchar, pchar name, ConfigField &);
#endif
}
