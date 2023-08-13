// 2023/05/21 10:21:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/ReaderFTP.h"


namespace Programmer
{
    // Подгтовка к программированию - стирание памяти c адреса
    void Prepare(uint address);

    // size - реальный размер находящихся в буфере данных
    void WriteBytes(char [ReaderFTP::SIZE_DATA_BUFFER], int size);

    // Нужно вызывать эту функцию после записи всех байт, чтобы записать буфер
    void CloseSession();

    // Столько байт уже записано
    int WrittenBytes();

    void CopyFirmware();
}
