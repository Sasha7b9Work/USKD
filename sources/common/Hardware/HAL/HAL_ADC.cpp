// 2023/04/03 19:52:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Measurer/Measurer.h"
#include <gd32f30x.h>
#include <systick.h>


/*
    ADC012_IN0      23  PA0     volt 1
    ADC012_IN1      24  PA1     volt 2
    ADC012_IN2      25  PA2     volt 3
    ADC012_IN3      26  PA3     cur l 1
    ADC012_IN4      29  PA4     cur h 1
    ADC012_IN5      30  PA5     cur l 2
    ADC012_IN6      31  PA6     cur h 2
    ADC012_IN7      32  PA7     cur l 3
    ADC012_IN8      35  PB0
    ADC012_IN9      36  PB1
    ADC012_IN10     15  PC0
    ADC012_IN11     16  PC1
    ADC012_IN12     17  PC2
    ADC012_IN13     18  PC3
    ADC012_IN14     33  PC4     cur h 3
    ADC012_IN15     34  PC5
*/


namespace HAL_ADC
{
    static uint16 adc_values[6] = { 0, 0, 0, 0, 0, 0 };
}


void HAL_ADC::Callback()
{
    Measurer::AppendMeasures(adc_values);
}


void HAL_ADC::Init()
{
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);

    nvic_irq_enable(ADC0_1_IRQn, 0, 0);

    pinVolt1._Init();
    pinVolt2._Init();
    pinVolt3._Init();

    pinCur1L._Init();
    pinCur2L._Init();
    pinCur3L._Init();

    ///////////////////////////////////////////////////////////////////////////////////////////// DMA
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(&adc_values);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 6;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);

    dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);

    dma_channel_enable(DMA0, DMA_CH0);

    /////////////////////////////////////////////////////////////////////////////////////////// TIM
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    timer_initpara.prescaler = 30000;               // Получаем частоту 8 МГц / 480 = 16.(6) кГц. На этой частоте 2000 точек считываются за 120 мс
    timer_initpara.alignedmode = TIMER_COUNTER_DOWN;
    timer_initpara.counterdirection = TIMER_COUNTER_DOWN;
    timer_initpara.period = 0xFFF;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV4;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    timer_channel_output_struct_para_init(&timer_ocintpara);

    /* CH0 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 4094);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* TIMER0 primary output enable */
    timer_primary_output_config(TIMER0, ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_disable(TIMER0);

    timer_enable(TIMER0);

    /////////////////////////////////////////////////////////////////////////////////////////////// ADC
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);

    /* ADC continuous function enable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);

    /* ADC scan function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);

    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 6);

    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0, pinVolt1.Channel(), ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 1, pinVolt2.Channel(), ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 2, pinVolt3.Channel(), ADC_SAMPLETIME_239POINT5);

    adc_regular_channel_config(ADC0, 3, pinCur1L.Channel(), ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 4, pinCur2L.Channel(), ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, 5, pinCur3L.Channel(), ADC_SAMPLETIME_239POINT5);

    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_T0_CH0);

    adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOC);
    adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);

    adc_interrupt_enable(ADC0, ADC_INT_EOC);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(1);

    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

    adc_dma_mode_enable(ADC0);

    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}


void HAL_ADC::DeInit()
{
    adc_disable(ADC0);

    adc_interrupt_disable(ADC0, ADC_INT_EOC);

    timer_disable(TIMER0);

    timer_deinit(TIMER0);

    dma_deinit(DMA0, DMA_CH0);

    nvic_irq_enable(ADC0_1_IRQn, 0, 0);
}
