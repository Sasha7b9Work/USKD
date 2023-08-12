// 2023/04/05 23:37:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT/Sender/GP.h"
#include "Settings/Settings.h"
#include <gd32f30x.h>
#include <cstdlib>


PinADC pinVolt1(GPIOA, GPIO_PIN_0, ADC_CHANNEL_0);
PinADC pinVolt2(GPIOA, GPIO_PIN_1, ADC_CHANNEL_1);
PinADC pinVolt3(GPIOA, GPIO_PIN_2, ADC_CHANNEL_2);

PinADC pinCur1L(GPIOA, GPIO_PIN_3, ADC_CHANNEL_3);
PinADC pinCur2L(GPIOA, GPIO_PIN_5, ADC_CHANNEL_5);
PinADC pinCur3L(GPIOA, GPIO_PIN_7, ADC_CHANNEL_7);

PinADC pinCur1H(GPIOA, GPIO_PIN_4, ADC_CHANNEL_4);
PinADC pinCur2H(GPIOA, GPIO_PIN_6, ADC_CHANNEL_6);
PinADC pinCur3H(GPIOC, GPIO_PIN_4, ADC_CHANNEL_14);

const uint I2C_ADDR = I2C0;
PinI2C pinI2C_SCL;
PinI2C pinI2C_SDA;

const uint USART_GPRS_ADDR = UART3;
PinUSART_TX pinUSART_GPRS_TX;
PinUSART_RX pinUSART_GPRS_RX;

const uint USART_LOG_ADDR = USART1;
PinUSART_TX pinUSART_LOG_TX;
PinUSART_RX pinUSART_LOG_RX;

PinOUT pinGSM_PWR;
PinOUT pinGSM_PWRKEY;
PinIN  pinGSM_STATUS;

PinOUT pinMX0;
PinOUT pinMX1;
PinOUT pinMX2;
PinOUT pinMX3;
PinOUT pinMX4;

PinIN pinP1;
PinIN pinP2;

PinOUT pinKMA1;
PinOUT pinKMA2;
PinOUT pinKMA3;
PinOUT pinKMA4;
PinOUT pinKMA5;
PinOUT pinKMA6;
PinOUT pinKMA7;
PinOUT pinKMA8;
PinOUT pinKMA9;
PinOUT pinKMB1;
PinOUT pinKMB2;
PinOUT pinKMB3;
PinOUT pinKMB4;
PinOUT pinKMB5;
PinOUT pinKMB6;
PinOUT pinKMB7;
PinOUT pinKMB8;
PinOUT pinKMB9;
PinOUT pinKMC1;
PinOUT pinKMC2;
PinOUT pinKMC3;
PinOUT pinKMC4;
PinOUT pinKMC5;
PinOUT pinKMC6;
PinOUT pinKMC7;
PinOUT pinKMC8;
PinOUT pinKMC9;

PinIN pinLevel4;


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


static ObservedPin pinsGP[3];

void HAL_PINS::Init()
{
    uint pins[3] = { GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_2 };

    for (int i = 0; i < 3; i++)
    {
        pinsGP[i]._Init(GPIOC, pins[i], GPIO_MODE_IPU);
    }

    pinLevel4._Init(GPIOC, GPIO_PIN_9, GPIO_MODE_IPU);
}


void HAL_PINS::Update()
{
#ifdef DEVICE
    for (int i = 0; i < 3; i++)
    {
        pinsGP[i].IsHi();

        if (pinsGP[i].IsSwitched())
        {
            Sender::GP::Send(i + 1, !pinsGP[i].GetState());

            pinsGP[i].ResetSwitch();
        }
    }
#endif
}


void HAL_PINS::SendState()
{
#ifdef DEVICE
    for (int i = 0; i < 3; i++)
    {
        pinsGP[i].IsHi();

        Sender::GP::Send(i + 1, !pinsGP[i].GetState());

        pinsGP[i].ResetSwitch();
    }
#endif
}


void PinADC::_Init()
{
    gpio_init(port, GPIO_MODE_AIN, GPIO_OSPEED_MAX, pin);
}


void PinI2C::_Init(uint _port, uint _pin)
{
    port = _port;
    pin = _pin;

    if (gset.OnlyMeasure())
    {
        gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
    }

    gpio_init(port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, pin);
}


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
