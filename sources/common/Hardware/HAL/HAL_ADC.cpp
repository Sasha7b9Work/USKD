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

    /* ADC continuous function enable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);

    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC resolusion 6B */
    adc_resolution_config(ADC0, ADC_RESOLUTION_6B);

    /* enable ADC interface */
    adc_enable(ADC0);
    Timer::DelayMS(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}


float HAL_ADC::GetCharge()
{
    adc_flag_clear(ADC0, ADC_FLAG_EOC);

    while (SET != adc_flag_get(ADC0, ADC_FLAG_EOC))
    {
    }

    voltage = ConversionRawToVoltageBattery(ADC_RDATA(ADC0));

    float charge = 0.0f;

    if (voltage <= 3.7f)
    {
        charge = 0.0f;
    }
    else if(voltage >= 4.1f)
    {
        charge = 100.0f;
    }
    else
    {
        charge = (voltage - 3.7f) * 250.0f;
    }

    return charge;
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
