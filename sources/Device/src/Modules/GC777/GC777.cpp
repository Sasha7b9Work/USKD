// 2023/10/09 16:43:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/GC777/GC777.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


namespace GC777
{
    void WithoutIT();
}


void GC777::Init()
{
//    WithoutIT();

    pinGC777.Init();

    uint reg = GPIO_OCTL(GPIOC);

    _SET_BIT(reg, 9);

    GPIO_OCTL(GPIOC) = reg;

    nvic_irq_enable(EXTI5_9_IRQn, 2, 0);

    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_9);

    exti_init(EXTI_9, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_9);
}


void GC777::WithoutIT()
{
    PinIN pin(GPIOC, GPIO_PIN_9, GPIO_MODE_IPU);

    pin.Init();

    while (true)
    {
        if (pin.IsHi())
        {
//            int i = 0;
        }

        if (pin.IsLow())
        {
//            int i = 0;
        }
    }
}
