// 2023/04/05 23:37:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>
#include <cstdlib>


const uint USART_GPRS_ADDR = UART3;
PinUSART_TX pinUSART_GPRS_TX;
PinUSART_RX pinUSART_GPRS_RX;

const uint USART_LOG_ADDR = USART1;
PinUSART_TX pinUSART_LOG_TX;
PinUSART_RX pinUSART_LOG_RX;

PinOUT pinGSM_PWR;
PinOUT pinGSM_PWRKEY;
PinIN  pinGSM_STATUS;


struct ObservedPin : public PinIN
{
    ObservedPin() : PinIN(), state(false) { }

    bool IsHi()
    {
        prev_state = state;
        state = PinIN::IsHi();
        return state;
    }

    bool IsSwitched() const
    {
        return state != prev_state;
    }

    void ResetSwitch()
    {
        prev_state = state;
    }

    bool GetState() const
    {
        return state;
    }

private:
    bool state;
    bool prev_state;
};


void PinUSART_TX::_Init(uint _port, uint _pin)
{
    port = _port;
    pin = _pin;

    gpio_init(port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pin);
}


void PinUSART_RX::_Init(uint _port, uint _pin)
{
    port = _port;
    pin = _pin;

    gpio_init(port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, pin);
}


void PinOUT::_Init(uint _port, uint _pin)
{
    port = _port;
    pin = _pin;

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


void PinIN::_Init(uint _port, uint _pin, uint input_mode)
{
    port = _port;
    pin = _pin;

    gpio_init(port, input_mode, GPIO_OSPEED_50MHZ, pin);
}


void PinIN::DeInit()
{
    gpio_init(port, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, pin);

    // ��������� � ����
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
