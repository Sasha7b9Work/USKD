// 2023/04/13 15:40:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include <gd32f30x.h>


void i2c_start_on_bus(uint32_t i2c_periph)
{

}


void i2c_stop_on_bus(uint32_t i2c_periph)
{

}



void i2c_mode_addr_config(uint32_t i2c_periph, uint32_t mode, uint32_t addformat, uint32_t addr)
{

}


void i2c_data_transmit(uint32_t i2c_periph, uint8_t data)
{

}


void i2c_clock_config(uint32_t i2c_periph, uint32_t clkspeed, uint32_t dutycyc)
{

}


void i2c_master_addressing(uint32_t i2c_periph, uint32_t addr, uint32_t trandirection)
{

}


void i2c_ack_config(uint32_t i2c_periph, uint32_t ack)
{

}


void i2c_enable(uint32_t i2c_periph)
{

}


FlagStatus i2c_flag_get(uint32_t i2c_periph, i2c_flag_enum flag)
{
    return RESET;
}


void i2c_flag_clear(uint32_t i2c_periph, i2c_flag_enum flag)
{

}
