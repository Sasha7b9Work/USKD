// 2023/04/07 17:06:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// Описывает сектор
struct Sector               // 64 kB
{
    static const int SIZE = 64 * 1024;

    Sector(uint _number);
    uint Number() const { return number; }
    // Адрес байта, следующего за последним байтом сектора
    uint End() const;
    // true, если адрес принадлежит сектору
    bool AddressBelong(uint address) const;
    // Возвращает сектор, содержащий address
    static Sector ForAddress(uint address);
private:
    uint number;    // Номер сектора
    uint begin;     // Начало сектора
    uint end;       // Конец сектора (адрес за последним байтом сектора)
};


namespace M25P80
{
    void EraseSector(uint num_sector);

    void Write(uint address, void *buffer, int size);

    void Read(uint address, void *buffer, int size);

    // Записать байт по нулевому адресу
    void WriteByte(uint address, uint8 byte);

    // Читать байт из нулевого адреса
    uint8 ReadByte(uint address);
}
