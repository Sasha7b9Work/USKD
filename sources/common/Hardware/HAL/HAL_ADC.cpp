// 2023/08/14 11:09:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>


namespace HAL_ADC
{
    static float voltage = 0.0f;

    static float ConversionRawToVoltageADC(uint);

    static float ConversionVoltageADCtoVoltageBattery(float);

    static float ConversionRawToVoltageBattery(uint);
}


void HAL_ADC::Init()
{
    pinBAT.Init();

    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);

    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 2);

    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_1, ADC_SAMPLETIME_239POINT5);

    adc_enable(ADC0);

    adc_software_trigger_enable(ADC0, ADC_CHANNEL_1);

    Timer::DelayMS(1);

    adc_calibration_enable(ADC1);
}


float HAL_ADC::GetVoltage()
{
    if (adc_flag_get(ADC0, ADC_FLAG_EOC) == SET)
    {
        voltage = ConversionRawToVoltageBattery(ADC_RDATA(ADC0));

        adc_flag_clear(ADC0, ADC_FLAG_EOC);
    }

    return voltage;
}


float HAL_ADC::ConversionRawToVoltageADC(uint raw)
{
    static const float MAX_VOLTAGE = 3.0f;

    return MAX_VOLTAGE / (1 << 12) * (float)raw;
}


float HAL_ADC::ConversionVoltageADCtoVoltageBattery(float adc)
{
    return -0.6f * adc + 4.36f;
}


float HAL_ADC::ConversionRawToVoltageBattery(uint raw)
{
    float voltageADC = ConversionRawToVoltageADC(raw);

    return ConversionVoltageADCtoVoltageBattery(voltageADC);
}
