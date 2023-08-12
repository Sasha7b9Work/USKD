// 2023/05/24 12:13:36 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Bootloader.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


namespace Bootloader
{
    //set Main Stack value
    __asm void MSR_MSP(uint)
    {
#ifndef WIN32
        MSR MSP, r0
        BX r14
#endif
    }
}


void Bootloader::Run()
{
    HAL::DeInit();

    const uint ADDR_BOOTLOADER = 0x08026000U;

    typedef void (*iapfun)(void);
    iapfun  jump2app;

    jump2app = (iapfun) * (volatile uint *)(ADDR_BOOTLOADER + 4); //-V566
    MSR_MSP(*(volatile uint *)ADDR_BOOTLOADER);                        //initialize app pointer //-V566
    nvic_vector_table_set(ADDR_BOOTLOADER, 0);
    jump2app();                                                                 //jump to app
}
