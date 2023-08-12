// 2023/04/13 15:40:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include <gd32f30x.h>


void fmc_lock(void)
{
}


void fmc_unlock(void)
{
}


fmc_state_enum fmc_page_erase(uint32_t page_address)
{
    return FMC_READY;
}


fmc_state_enum fmc_word_program(uint32_t address, uint32_t data)
{
    return FMC_READY;
}


void fmc_flag_clear(uint32_t flag)
{

}
