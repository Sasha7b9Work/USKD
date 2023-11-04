// 2023/04/06 13:08:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include <gd32f30x.h>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>


void HAL_USART_GPRS::Init()
{
    pinUSART_GPRS_TX.Init();
    pinUSART_GPRS_RX.Init();

    nvic_irq_enable(UART3_IRQn, 1, 1);

    NVIC_SetPriority(UART3_IRQn, 1);

    usart_deinit(USART_GPRS_ADDR);
    usart_baudrate_set(USART_GPRS_ADDR, 115200);
    usart_parity_config(USART_GPRS_ADDR, USART_PM_NONE);
    usart_word_length_set(USART_GPRS_ADDR, USART_WL_8BIT);
    usart_stop_bit_set(USART_GPRS_ADDR, USART_STB_1BIT);
    usart_receive_config(USART_GPRS_ADDR, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART_GPRS_ADDR, USART_TRANSMIT_ENABLE);

    usart_interrupt_enable(USART_GPRS_ADDR, USART_INT_RBNE);

    usart_enable(USART_GPRS_ADDR);
}


void HAL_USART_GPRS::DeInit()
{
    usart_disable(USART_GPRS_ADDR);

    usart_interrupt_disable(USART_GPRS_ADDR, USART_INT_RBNE);

    usart_deinit(USART_GPRS_ADDR);

    nvic_irq_disable(UART3_IRQn);

    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);

    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_11);

    GPIO_BC(GPIOC) = GPIO_PIN_10;

    GPIO_BC(GPIOC) = GPIO_PIN_11;
}


void HAL_USART_GPRS::Transmit(pchar message)
{
    Transmit((void *)message, (int)std::strlen(message));
}


void HAL_USART_GPRS::Transmit(void *buffer, int size)
{
    uint8 *data = (uint8 *)buffer;

    for (int i = 0; i < size; i++)
    {
        usart_data_transmit(USART_GPRS_ADDR, (uint16)*data++);

        while (RESET == usart_flag_get(USART_GPRS_ADDR, USART_FLAG_TBE)) {};
    }
}


void HAL_USART_GPRS::CallbackOnReceive(char symbol)
{
    Modem::CallbackOnReceive(symbol);
}


void HAL_USART_LOG::Init()
{
    if (!HAL::IsLayout())
    {
#ifndef HOME_VERSION
        return;
#endif
    }

    pinUSART_LOG_TX.Init();
    pinUSART_LOG_RX.Init();

#ifdef HOME_VERSION

    gpio_pin_remap_config(GPIO_USART1_REMAP, ENABLE);

#endif

    usart_deinit(USART_LOG_ADDR);
    usart_baudrate_set(USART_LOG_ADDR, 115200);
    usart_transmit_config(USART_LOG_ADDR, USART_TRANSMIT_ENABLE);

    usart_enable(USART_LOG_ADDR);
}


void HAL_USART_LOG::Transmit(pchar format, ...)
{
    if (!HAL::IsLayout())
    {
#ifndef HOME_VERSION
        return;
#endif
    }

    char message[2048];
    std::va_list args;
    va_start(args, format);
    vsprintf(message, format, args);
    va_end(args);

    uint size = std::strlen(message);

    if (size < 2 || (message[size - 2] != 0x0d && message[size - 1] != 0x0a))
    {
        std::strcat(message, "\r\n");
    }

    TransmitRAW(message);
}


void HAL_USART_LOG::TransmitRAW(pchar message)
{
    if (!HAL::IsLayout())
    {
#ifndef HOME_VERSION
        return;
#endif
    }

    if (message[0] == 0x0D && message[1] == 0x0A)
    {
        message += 2;
    }

    int size = (int)std::strlen(message);

    for (int i = 0; i < size; i++)
    {
        while (RESET == usart_flag_get(USART_LOG_ADDR, USART_FLAG_TBE)) {};

        usart_data_transmit(USART_LOG_ADDR, (uint16)message[i]);

        while (RESET == usart_flag_get(USART_LOG_ADDR, USART_FLAG_TBE)) {};
    }
}
