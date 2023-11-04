// 2023/04/05 13:04:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


void HAL_ROM::ErasePage(int num_page)
{
    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

    fmc_page_erase(FLASH_BASE + num_page * SIZE_PAGE);

    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

    fmc_lock();
}


bool HAL_ROM::PageIsEmpty(int num_page)
{
    uint *start = (uint *)(FLASH_BASE + num_page * SIZE_PAGE);

    uint *end = start + SIZE_PAGE;

    for (uint *address = start; address < end; address += 4)
    {
        if (*address != (uint)(-1))
        {
            return false;
        }
    }

    return true;
}


void HAL_ROM::WriteUInt(uint address, uint value)
{
    WriteData(address, &value, sizeof(value));
}


void HAL_ROM::WriteData(uint address, const void *_data, int size)
{
    const uint8 *data = (const uint8 *)_data;

    fmc_unlock();

    while (size % 4)
    {
        size++;
    }

    uint end = address + size;

    while (address < end)
    {
        fmc_word_program(address, *(uint *)data);
        data += 4;
        address += 4;

        fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
    }

    fmc_lock();
}
