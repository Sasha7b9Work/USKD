// 2023/08/24 21:29:38 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/LED.h"
#include <gd32f30x.h>


LED ledRED(GPIOC, GPIO_PIN_7);
LED ledYELLOW(GPIOC, GPIO_PIN_8);
LED ledGREEN(GPIOC, GPIO_PIN_9);


void LED::Init()
{
    gpio_init(port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, pin);

    Off();
}


void LED::On()
{
    GPIO_BOP(port) = pin;
}


void LED::Off()
{
    GPIO_BC(port) = pin;
}
