// 2023/04/07 16:39:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>

/*
*   SPI1    Alternate
*   CS      84  PD3
*   SCK     89  PB3
*   MISO    90  PB4
*   MOSI    91  PB5
*/


namespace HAL_SPI2
{
    const uint SPI_ADDR = SPI2;

    namespace CS
    {
        void Hi()
        {
            GPIO_BOP(GPIOD) = GPIO_PIN_3;
        }

        void Low()
        {
            GPIO_BC(GPIOD) = GPIO_PIN_3;
        }

        void Init()
        {
            gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

            Hi();
        }
    }
}


void HAL_SPI2::Init()
{
    CS::Init();

    // SCK, MOSI
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_5);

    // MISO
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

    spi_parameter_struct is;

    is.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    is.device_mode = SPI_MASTER;
    is.frame_size = SPI_FRAMESIZE_8BIT;
    is.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
    is.nss = SPI_NSS_SOFT;
    is.prescale = SPI_PSC_32;
    is.endian = SPI_ENDIAN_MSB;
    spi_init(SPI_ADDR, &is);

    spi_enable(SPI_ADDR);
}


void HAL_SPI2::Write(uint8 byte)
{
    CS::Low();

    while(RESET == spi_i2s_flag_get(SPI_ADDR, SPI_FLAG_TBE)) { }

    spi_i2s_data_transmit(SPI_ADDR, byte);

    CS::Hi();
}


void HAL_SPI2::Write(const void *buffer, int size)
{
    CS::Low();

    const uint8 *data = (const uint8 *)buffer;

    for (int i = 0; i < size; i++)
    {
        while (RESET == spi_i2s_flag_get(SPI_ADDR, SPI_FLAG_TBE)) {}

        spi_i2s_data_transmit(SPI_ADDR, data[i]);
    }

    CS::Hi();
}


void HAL_SPI2::WriteRead(const void *_out, void *_in, int size)
{
    CS::Low();

    const uint8 *out = (const uint8 *)_out;
    uint8 *in = (uint8 *)_in;

    for (int i = 0; i < size; i++)
    {
        while(RESET == spi_i2s_flag_get(SPI_ADDR, SPI_FLAG_TBE)) { }

        spi_i2s_data_transmit(SPI_ADDR, out[i]);

        while(RESET == spi_i2s_flag_get(SPI_ADDR, SPI_FLAG_RBNE)) { }

        in[i] = (uint8)spi_i2s_data_receive(SPI_ADDR);
    }

    CS::Hi();
}
