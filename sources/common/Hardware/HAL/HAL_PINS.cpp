// 2023/04/05 23:37:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>
#include <cstdlib>


PinADC pinBAT(GPIOA, GPIO_PIN_1, ADC_CHANNEL_1);

const uint USART_GPRS_ADDR = UART3;
PinUSART_TX pinUSART_GPRS_TX(GPIOC, GPIO_PIN_10);
PinUSART_RX pinUSART_GPRS_RX(GPIOC, GPIO_PIN_11);

#ifdef HOME_VERSION
    const uint USART_LOG_ADDR = USART1;
    PinUSART_TX pinUSART_LOG_TX(GPIOD, GPIO_PIN_5);
    PinUSART_RX pinUSART_LOG_RX(GPIOD, GPIO_PIN_6);
#else
    const uint USART_LOG_ADDR = USART0;
    PinUSART_TX pinUSART_LOG_TX(GPIOB, GPIO_PIN_6);
    PinUSART_RX pinUSART_LOG_RX(GPIOB, GPIO_PIN_7);
#endif

PinOUT pinGSM_PWR(GPIOA, GPIO_PIN_12);
PinOUT pinGSM_PWRKEY(GPIOA, GPIO_PIN_11);
PinIN  pinGSM_STATUS(GPIOA, GPIO_PIN_10, GPIO_MODE_IPD);

PinIN pinGC777(GPIOC, GPIO_PIN_9, GPIO_MODE_IN_FLOATING);


void PinUSART_TX::Init()
{
    gpio_init(port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pin);
}


void PinUSART_RX::Init()
{
    gpio_init(port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, pin);
}


void PinOUT::Init()
{
    gpio_init(port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, pin);
}


void PinOUT::Set()
{
    GPIO_BOP(port) = pin;
}


void PinOUT::Reset()
{
    GPIO_BC(port) = pin;
}


void PinOUT::SetState(bool state)
{
    state ? Set() : Reset();
}


void PinIN::Init()
{
    gpio_init(port, mode, GPIO_OSPEED_50MHZ, pin);
}


void PinIN::DeInit()
{
    gpio_init(port, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, pin);

     // Переводим в ноль
    GPIO_BC(port) = pin;
}


bool PinIN::IsLow()
{
    return gpio_input_bit_get(port, pin) == RESET;
}


bool PinIN::IsHi()
{
    return !IsLow();
}


void PinI2C::Init(uint _port, uint _pin, uint _mode)
{
    port = _port;
    pin = _pin;

    gpio_init(port, _mode, GPIO_OSPEED_50MHZ, pin);
}


void PinADC::Init()
{
    gpio_init(port, GPIO_MODE_AIN, GPIO_OSPEED_MAX, pin);
}
