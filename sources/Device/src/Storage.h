// 2023/08/15 18:48:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// Здесь хранятся данные


struct TypeMeasure
{
    enum E
    {
        Altitude,
        Longitude,
        Temperature,
        Humidity,
        Count
    };
};


namespace Storage
{
    void Update();

    void Set(TypeMeasure::E, float);

    float Get(TypeMeasure::E);
}
