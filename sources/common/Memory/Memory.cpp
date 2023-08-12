// 2023/06/28 17:50:45 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Memory/Memory.h"
#include "Memory/M25P80.h"


bool Memory::Test()
{
    /*
    *  1. Стираем нулевой сектор сектор.
    *  2. Записываем во все байты нулевого сектора (uint8)address.
    *  3. Проверяем все байты
    */

    DEBUG_POINT;

    M25P80::EraseSector(0);

    DEBUG_POINT;

    for (uint i = 0; i < 0x10000; i++)
    {
        M25P80::WriteByte(i, (uint8)i);
    }

    DEBUG_POINT;

    for (uint i = 0; i < 0x10000; i++)
    {
        if (M25P80::ReadByte(i) != (uint8)i)
        {
            return false;
        }
    }

    DEBUG_POINT;

    return true;
}
