// 2023/08/14 11:09:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>


namespace HAL_ADC
{
    static float voltage = 0.0f;
}


void HAL_ADC::Init()
{
    pinBAT.Init();


    adc_mode_config(ADC_MODE_FREE);

    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);

    adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 2);

    adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_1, ADC_SAMPLETIME_239POINT5);

    adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_17, ADC_SAMPLETIME_239POINT5);

    adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);

    adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE);

    adc_enable(ADC0);

    Timer::DelayMS(1);

    adc_calibration_enable(ADC0);
}


float HAL_ADC::GetVoltage()
{
    return voltage;
}
