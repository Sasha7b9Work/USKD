// 2023/05/30 11:08:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SIM868.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/MQTT/MQTT.h"
#include <cstdio>
#include <cstring>


void SIM800::Transmit::With0D(pchar message)
{
    if (std::strlen(message) > 2
#ifdef DEVICE
        && MQTT::InStateIdle()
#endif
        )
    {
        LOG_WRITE(">>> %s", message);
    }

    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);
}


void SIM800::Transmit::RAW(pchar message)
{
    HAL_USART_GPRS::Transmit(message);
}


void SIM800::Transmit::UINT8(uint8 byte)
{
    HAL_USART_GPRS::Transmit(&byte, 1);
}


void SIM800::Transmit::Format(pchar format, pchar param)
{
    char buffer[64];
    std::sprintf(buffer, (char *)format, param);
    SIM800::Transmit::With0D(buffer);
}


void SIM800::Transmit::Format(pchar format, int param)
{
    char buffer[64];
    std::sprintf(buffer, (char *)format, param);
    SIM800::Transmit::With0D(buffer);
}
