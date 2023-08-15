// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Storage.h"


namespace Modem
{
    void Reset();

    void Init();

    void Update();

    // Есть обновление
    bool ExistUpdate();

    void CallbackOnReceive(char);

    void SendMeasuremets(float [TypeMeasure::Count]);

    namespace Mode
    {
        // Возвращает true, если напряжение подано
        bool Power();
    }
}
