// 2023/09/05 21:00:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Route
{
    // Вызываем при старте поездки
    void Start();

    // Вызываем нри завершении
    void End();

    // Время поездки в секундах
    uint TimeInRouteInMin();

    bool InRoute();
}
