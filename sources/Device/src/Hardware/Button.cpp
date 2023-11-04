// 2023/08/24 21:30:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Button.h"
#include <gd32f30x.h>


void Button::Init()
{
    gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
}


void Button::Update()
{
    if (gpio_input_bit_get(GPIOC, GPIO_PIN_6) == RESET)
    {
        // Нажата
    }
}
