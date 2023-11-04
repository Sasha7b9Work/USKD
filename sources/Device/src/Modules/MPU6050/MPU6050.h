// 2023/08/18 14:41:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct DataMPU6050
{
    DataMPU6050();

    float g[3];
    float dps[3];
    float temp;

    // –ассчитывает модуль динамического ускорени€
    float CalculateHit() const;
};


namespace MPU6050
{
    void _Init();

    DataMPU6050 _ReadData();
}
