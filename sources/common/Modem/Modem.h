// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

#ifdef DEVICE
    #include "Storage/Storage.h"
#endif


namespace Modem
{
    void Reset(char *file, int line);

    void Init();

    void Update();

    // Есть обновление
    bool ExistUpdate();

    void CallbackOnReceive(char);

    namespace Mode
    {
        // Возвращает true, если напряжение подано
        bool Power();
    }

#ifdef DEVICE

    // Заслать измерения на сервера
    void SendMeasures(const Measurements &);

#endif
}
