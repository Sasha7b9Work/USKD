// 2023/08/18 14:41:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/MPU6050/MPU6050.h"
#include "Modules/MPU6050/driver_mpu6050_basic.h"
#include <cstring>
#include <cmath>


DataMPU6050::DataMPU6050()
{
    std::memset(this, 0, sizeof(*this));
}


float DataMPU6050::CalculateHit() const
{
    return std::sqrtf(dps[0] * dps[0] + dps[1] * dps[1] + dps[2] * dps[2]);
}


void MPU6050::_Init()
{
    mpu6050_basic_init(MPU6050_ADDRESS_AD0_LOW);
}


DataMPU6050 MPU6050::_ReadData()
{
    DataMPU6050 result;

    mpu6050_basic_read(result.g, result.dps);

    mpu6050_basic_read_temperature(&result.temp);

    return result;
}
