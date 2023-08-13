// 2023/05/21 11:05:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace ReaderFTP
{
    static const int SIZE_DATA_BUFFER = 64;

    void Reset();

    void ReceiveBytes(int num_bytes);

    void AppendByte(char symbol);

    extern bool requested_bytes_received;       // Признак того, что запрошенные байты получены
    extern bool received_FTPGET_1_0;            // Признак того, что файл полностью прочитан. Сессия закрыта
    extern int pointer_data;                    // Столько байт уже принято
    extern char buffer_data[SIZE_DATA_BUFFER];  // Здесь принятые данные
}
