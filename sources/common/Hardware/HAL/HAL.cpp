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

    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_I2C0);      // Display
    rcu_periph_clock_enable(RCU_I2C1);      // MPU6050
    rcu_periph_clock_enable(RCU_TIMER0);    // ADC

#ifdef HOME_VERSION
    rcu_periph_clock_enable(RCU_USART1);    // Log
#else
    rcu_periph_clock_enable(RCU_USART0);    // Log
#endif
    rcu_periph_clock_enable(RCU_UART3);     // GPRS
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER5);

#ifndef HOME_VERSION
    gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
#endif

    GL::_RCU_RSTSCK = *((uint *)(0x40021000 + 0x24)); //-V566

    if (RESET != rcu_flag_get(RCU_FLAG_FWDGTRST)) {
        /* clear the FWDGT reset flag */
        rcu_all_reset_flag_clear();
    }

    rcu_periph_clock_enable(RCU_WWDGT);

    HAL_USART_GPRS::Init();

    HAL_USART_LOG::Init();

#ifdef HOME_VERSION
    i2cDisplay.Init(GPIOB, GPIO_PIN_6, GPIO_PIN_7, HAL_I2C::PERIPH_DISPLAY, (0x3c << 1));
#else
    i2cDisplay.Init(GPIOB, GPIO_PIN_8, GPIO_PIN_9, HAL_I2C::PERIPH_DISPLAY, (0x3c << 1));
#endif

//    i2cMPU6050.Init(GPIOB, GPIO_PIN_10, GPIO_PIN_11, HAL_I2C::PERIPH_MPU6050, (0x68 << 1));

//    HAL_FWDGT::Init();

    HAL_ADC::Init();
}


void HAL::DeInit()
{
    HAL_USART_GPRS::DeInit();
}


pchar HAL::GetUID()
{
    static char buffer[32] = { 0 };

    if (buffer[0] == 0)
    {
        uint8 bytes[12];

        std::memcpy(bytes, (void *)0x1FFFF7E8, 12); //-V566

        std::sprintf(buffer, "%08X", Math::CalculateCRC((uint)bytes, 12));
    }

    return buffer;
}


bool HAL::IsLayout()
{
    static bool result = (std::strcmp(HAL::GetUID(), "F95562E2") == 0);

    return result;
}


void HAL::ErrorHandler()
{
    while (1)
    {
    }
}
