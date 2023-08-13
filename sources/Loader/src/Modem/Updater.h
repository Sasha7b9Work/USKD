// 2023/05/18 13:54:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Updater
{
    // Возвращает true, если Updater отработал (загружено обновление либо обновление не требуется)
    bool IsCompleted();

    void CallbackByteFromFTP(char);

    // Если true - принимаем байты из ftp-сервера
    bool InModeReceiveDataFromFTP();
}
