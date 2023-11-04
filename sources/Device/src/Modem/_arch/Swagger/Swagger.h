// 2023/05/18 13:54:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Storage/Storage.h"


namespace Swagger
{
    void Update(pchar);

    void GetServerTime();

    void GetConfig();

    void StartRoute();

    void StopRoute();

    void SendMeasuremets(const Measurements &);

    void SendIncident(pchar);
}
