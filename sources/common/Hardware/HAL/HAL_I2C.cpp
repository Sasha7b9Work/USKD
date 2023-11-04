// 2023/5/8 19:15:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>


HAL_I2C i2cDisplay;


#define I2C_SPEED              100000


const uint HAL_I2C::PERIPH_DISPLAY = I2C0;
const uint HAL_I2C::PERIPH_MPU6050 = I2C1;


bool HAL_I2C::WaitFlagYes(/*i2c_flag_enum*/ uint flag)
{
    TimeMeterMS meter;

    while (i2c_flag_get(periph, (i2c_flag_enum)flag))
    {
        if (meter.ElapsedTime() > TIMEOUT)
        {
            return false;
        }
    }

    return true;
}


bool HAL_I2C::WaitFlagNo(/*i2c_flag_enum*/ uint flag)
{
    TimeMeterMS meter;

    while (!i2c_flag_get(periph, (i2c_flag_enum)flag))
    {
        if (meter.ElapsedTime() > TIMEOUT)
        {
            return false;
        }
    }

    return true;
}


void HAL_I2C::Init(uint _port, uint _pinSCL, uint _pinSDA, uint _periph, uint _address)
{
    periph = _periph;
    address = _address;

    scl.Init(_port, _pinSCL, GPIO_MODE_AF_PP);
    sda.Init(_port, _pinSDA, GPIO_MODE_AF_OD);

    /* configure I2C clock */
    i2c_clock_config(periph, I2C_SPEED, I2C_DTCY_2);

    i2c_mode_addr_config(periph, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, address);

    /* enable I2C_ADDR */
    i2c_enable(periph);

    i2c_ack_config(periph, I2C_ACK_ENABLE);

    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(I2C0_ER_IRQn, 0, 1);

    i2c_software_reset_config(periph, I2C_CTL0_SRESET);
}


bool HAL_I2C::Write(uint8 command, uint8 *data, int size)
{
    TimeMeterMS meter;

    i2c_stop_on_bus(periph);

    /* wait until I2C bus is idle */
    WaitFlagYes(I2C_FLAG_I2CBSY);

    /* send a start condition to I2C bus */
    i2c_start_on_bus(periph);

    /* wait until SBSEND bit is set */
    WaitFlagNo(I2C_FLAG_SBSEND);

    /* send slave address to I2C bus */
    i2c_master_addressing(periph, address, I2C_TRANSMITTER);

    /* wait until ADDSEND bit is set */
    WaitFlagNo(I2C_FLAG_ADDSEND);

    /* N=1,reset ACKEN bit before clearing ADDRSEND bit */
    i2c_ack_config(periph, I2C_ACK_DISABLE);

    /* clear ADDSEND bit */
    i2c_flag_clear(periph, I2C_FLAG_ADDSEND);

    /* send command */
    i2c_data_transmit(periph, command);

    /* wait until the TBE bit is set */
    WaitFlagNo(I2C_FLAG_TBE);

    bool result = true;

    /* send array of data */
    for (int i = 0; i < size; i++)
    {
        i2c_data_transmit(periph, *data++);

        /* wait until the TBE bit is set */
        if (!WaitFlagNo(I2C_FLAG_TBE))
        {
            result = false;

            break;
        }
    }

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(periph);

    /* wait until stop condition generate */
    while (I2C_CTL0(periph) & 0x0200)
    {
        if (meter.ElapsedTime() > TIMEOUT)
        {
            result = false;

            break;
        }
    }

    /* Enable Acknowledge */
    i2c_ack_config(periph, I2C_ACK_ENABLE);

    return result;
}


bool HAL_I2C::Read(uint8 reg, uint8 *buf, uint16 len)
{
    TimeMeterMS meter;

    bool result = true;

    i2c_stop_on_bus(periph);

    /* wait until I2C bus is idle */
    WaitFlagYes(I2C_FLAG_I2CBSY);

    /* send a start condition to I2C bus */
    i2c_start_on_bus(periph);

    /* wait until SBSEND bit is set */
    WaitFlagNo(I2C_FLAG_SBSEND);

    /* send slave address to I2C bus */
    i2c_master_addressing(periph, address, I2C_TRANSMITTER);

    /* wait until ADDSEND bit is set */
    WaitFlagNo(I2C_FLAG_ADDSEND);

    /* clear ADDSEND bit */
    i2c_flag_clear(periph, I2C_FLAG_ADDSEND);

    /* send command */
    i2c_data_transmit(periph, reg);

    WaitFlagNo(I2C_FLAG_TBE);

    i2c_start_on_bus(periph);

    WaitFlagNo(I2C_FLAG_SBSEND);

    i2c_master_addressing(periph, address, I2C_RECEIVER);

    WaitFlagNo(I2C_FLAG_ADDSEND);

    i2c_flag_clear(periph, I2C_FLAG_ADDSEND);

    if (len == 1)
    {
        i2c_ack_config(periph, I2C_ACK_DISABLE);

        i2c_stop_on_bus(periph);

        while (!i2c_flag_get(periph, I2C_FLAG_RBNE))
        {
            if (meter.ElapsedTime() > TIMEOUT)
            {
                result = false;
                break;
            }
        }

        buf[0] = i2c_data_receive(periph);
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            if (i == len - 1)
            {
                i2c_ack_config(periph, I2C_ACK_DISABLE);
                i2c_stop_on_bus(periph);
            }

            while (!i2c_flag_get(periph, I2C_FLAG_RBNE))
            {
                if (meter.ElapsedTime() > TIMEOUT)
                {
                    result = false;
                    break;
                }
            }

            buf[i] = i2c_data_receive(periph);
        }
    }

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(periph);

    /* wait until stop condition generate */
    while (I2C_CTL0(periph) & 0x0200)
    {
        if (meter.ElapsedTime() > TIMEOUT)
        {
            result = false;
            break;
        }
    }

    /* enable acknowledge */
    i2c_ack_config(periph, I2C_ACK_ENABLE);

    return result;
}
