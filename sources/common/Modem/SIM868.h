// 2023/05/26 09:27:07 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct DataGPRS
{
    DataGPRS(float lat = 0.0f, float lon = 0.0f) : latitude(lat), longitude(lon) { }
    float latitude;
    float longitude;
};


namespace SIM868
{
    void Reset();

    void Update(pchar);

    // Возвращает true, если зарегистрирован в сети
    bool IsRegistered();

    pchar LevelSignal();

    namespace Transmit
    {
        // Передать с завершающим 0x0d
        void With0D(pchar);

        // Передать без завершающего 0x0d
        void RAW(pchar);

        void UINT8(uint8);

        void Format(pchar format, int param);

        void Format(pchar format, pchar param);
    }
}
