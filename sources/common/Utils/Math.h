// 2023/04/13 12:00:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Math
{
    template<class T>
    T CircularIncrease(T value, T min, T max)
    {
        T result = value + 1;

        return (result < max) ? result : min;
    }

    template<class T>
    T Limitation(T value, T min, T max)
    {
        if (value < min)
        {
            return min;
        }

        if (value > max)
        {
            return max;
        }

        return value;
    }

    inline uint Hash(uint hash, char byte)
    {
        return (uint8)byte + (hash << 6) + (hash << 16) - hash;
    }

    inline uint CalculateCRC(uint address, int size)
    {
        uint crc = 0;

        uint8 *data = (uint8 *)address;

        for (int i = 0; i < size; i++)
        {
            crc = Math::Hash(crc, (char)*data++);
        }

        return crc;
    }
}
