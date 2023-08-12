// 2023/04/07 17:06:22 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Memory/M25P80.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <cstring>


/*                         | MK  | FLASH |
    -----------------------+-----+-------+
    CS          - PD3 - 84 |     |       |
    SPI1 MISO   - PB4 - 90 |  I  |   2   |
    SPI1 SCK    - PB3 - 89 |     |       |
    SPI1 MOSI   - PB5 - 91 |  O  |   5   |
*/


/*
*   Block 0
*   Sector 0
*   Адреса 000000h - 0000FFh
*/

//                            page
#define PAGE_PROGRAM  0x02
#define READ_DATA     0x03
#define WRITE_DISABLE 0x04
#define READ_STATUS   0x05
#define WRITE_ENABLE  0x06
#define SECTOR_ERASE  0xD8


namespace M25P80
{
    // Записывает uin8, а затем младшие 3 байта из второго значения
    void Write32bit(uint8, uint);

    bool IsBusy();

    void WaitRelease();
}


void M25P80::EraseSector(uint num_sector)
{
    DEBUG_POINT;

    WaitRelease();

    DEBUG_POINT;

    HAL_SPI2::Write(WRITE_ENABLE);

    DEBUG_POINT;

    WaitRelease();

    DEBUG_POINT;

    Write32bit(SECTOR_ERASE, num_sector * 0x10000);

    DEBUG_POINT;

    WaitRelease();

    DEBUG_POINT;

    HAL_SPI2::Write(WRITE_DISABLE);

    DEBUG_POINT;
}


void M25P80::WriteByte(uint address, uint8 byte)
{
    uint8 data[5] = { PAGE_PROGRAM };

    BitSet32 bs(address);

    data[1] = bs.byte[2];
    data[2] = bs.byte[1];
    data[3] = bs.byte[0];
    data[4] = byte;

    WaitRelease();

    HAL_SPI2::Write(WRITE_ENABLE);

    HAL_SPI2::Write(data, 5);

    WaitRelease();

    HAL_SPI2::Write(WRITE_DISABLE);
}


uint8 M25P80::ReadByte(uint addreess)
{
    uint8 data[5] = { READ_DATA };

    BitSet32 bs(addreess);

    data[1] = bs.byte[2];
    data[2] = bs.byte[1];
    data[3] = bs.byte[0];
    data[4] = 0;

    uint8 in[5] = { 0, 0, 0, 0, 0 };

    HAL_SPI2::WriteRead(data, in, 5);

    return data[4];
}


void M25P80::Write(uint, void *, int)
{
}


void M25P80::Read(uint , void *, int )
{
}


void M25P80::Write32bit(uint8 command, uint bits24)
{
    uint8 data[4];

    BitSet32 bs(bits24);

    data[0] = command;
    data[1] = bs.byte[2];
    data[2] = bs.byte[1];
    data[3] = bs.byte[0];

    HAL_SPI2::Write(data, 4);
}


bool M25P80::IsBusy()
{
    static const uint8 out[2] = { READ_STATUS, 0 };
    static uint8 in[2] = { 0, 0 };

    HAL_SPI2::WriteRead(out, in, 2);

    LOG_WRITE("read %X", in[1]);

    return (in[1] & 0x01);
}


void M25P80::WaitRelease()
{
    TimeMeterMS meter;

    while (IsBusy())
    {
    }
}


Sector::Sector(uint _number) : number(_number)
{
    begin = _number * Sector::SIZE;
    end = begin + Sector::SIZE;
}


Sector Sector::ForAddress(uint address)
{
    uint num_sector = address / Sector::SIZE;

    return Sector(num_sector);
}


uint Sector::End() const
{
    return (Number() + 1) * Sector::SIZE;
}


bool Sector::AddressBelong(uint address) const
{
    return (address >= begin) && (address < end);
}
