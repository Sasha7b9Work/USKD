// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Utils/Math.h"
#include "Globals.h"
#include <gd32f30x.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>



/*
*   Задействованные ресурсы
*   ADC0     - измерение входных параметров
*   DMA0_CH0 - АЦП
*   TIM0_CH0 - АЦП
*   TIM1     - таймер мс
*/


void HAL::Init()
{
    systick_config();
    
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);

    rcu_periph_clock_enable(RCU_ADC0);      // Сбор информации
    rcu_periph_clock_enable(RCU_DMA0);      // Сбор информации
    rcu_periph_clock_enable(RCU_I2C0);      // Display
    rcu_periph_clock_enable(RCU_I2C1);
    rcu_periph_clock_enable(RCU_SPI2);      // Ext Flash  M25P80
    rcu_periph_clock_enable(RCU_TIMER0);    // ADC
    rcu_periph_clock_enable(RCU_USART1);    // Log
    rcu_periph_clock_enable(RCU_UART3);     // GPRS
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER1);    // Для опроса контакторв
    rcu_periph_clock_enable(RCU_TIMER5);

    GL::_RCU_RSTSCK = *((uint *)(0x40021000 + 0x24));

    if (RESET != rcu_flag_get(RCU_FLAG_FWDGTRST)) {
        /* clear the FWDGT reset flag */
        rcu_all_reset_flag_clear();
    }

    rcu_periph_clock_enable(RCU_WWDGT);

    HAL_I2C::Init();

    HAL_ADC::Init();

    HAL_USART_GPRS::Init();

    HAL_SPI2::Init();
    
    HAL_PINS::Init();

    HAL_FWDGT::Init();

#ifdef ENABLE_LOG
    HAL_USART_LOG::Init();
#endif
}


void HAL::DeInit()
{
    HAL_USART_GPRS::DeInit();

    HAL_ADC::DeInit();
}


char *HAL::GetUID(char buffer[32])
{
    uint8 bytes[12];

    std::memcpy(bytes, (void *)0x1FFFF7E8, 12);

    std::sprintf(buffer, "%X", Math::CalculateCRC((uint)bytes, 12));

    return buffer;
}


void HAL::ErrorHandler()
{
    while (1)
    {
    }
}
