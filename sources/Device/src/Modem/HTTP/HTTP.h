// 2023/05/18 13:54:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Storage.h"


namespace HTTP
{
    void Update(pchar);

    void SendMeasuremets(float[TypeMeasure::Count]);

    void Reset();
}
