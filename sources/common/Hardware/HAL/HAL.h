// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL_PINS.h"


struct PackedTime
{
    uint year    : 6;
    uint month   : 4;
    uint day     : 5;
    uint hours   : 5;
    uint minutes : 6;
    uint seconds : 6;
    PackedTime(uint h = 11, uint m = 11, uint s = 11, uint d = 11, uint mo = 11, uint y = 11) :
        year(y), month(mo), day(d), hours(h), minutes(m), seconds(s) {};
};


namespace HAL
{
    void Init();
    void DeInit();
    void ErrorHandler();
    char *GetUID(char buffer[32]);
}


namespace HAL_ADC
{
    void Init();
    float GetVoltage();
}


// Для дисплея
namespace HAL_I2C
{
    void Init();
    bool Write(uint8 command, uint8 *data, int size);
}


namespace HAL_USART_GPRS
{
    void Init();
    void DeInit();
    // Передать с завершающм 0x0d
    void Transmit(pchar);
    void Transmit(char);
    void Transmit(void *, int);
    void CallbackOnReceive(char);
}


namespace HAL_FWDGT
{
    void Init();

    // Установить рабочий режим
    void ToRunningMode();

    // Установить режим обновления
    void ToUpgradeMode();

    void Update();
}


namespace HAL_ROM
{
    static const uint ADDR_APPLICATION = 0x08000000U;
    static const uint ADDR_BASE = 0x08000000U;
    static const uint SIZE_PAGE = 2 * 1024;

    // В это место записывается новая прошивка перед заменой старой на неё
    static const uint ADDR_STORAGE = 0x8013000U;

    // num_sector от 0 до 127. Каждый сектор занимает 2 кБ
    void ErasePage(int num_page);

    // address должен быть кратен 4, size должен быть кратен 4
    void WriteData(uint address, uint8 *data, int size);

    bool PageIsEmpty(int num_page);
}


#ifdef __cplusplus
extern "C" {
#endif 

    /* function declarations */
    /* this function handles NMI exception */
    void NMI_Handler(void);
    /* this function handles HardFault exception */
    void HardFault_Handler(void);
    /* this function handles MemManage exception */
    void MemManage_Handler(void);
    /* this function handles BusFault exception */
    void BusFault_Handler(void);
    /* this function handles UsageFault exception */
    void UsageFault_Handler(void);
    /* this function handles SVC exception */
    void SVC_Handler(void);
    /* this function handles DebugMon exception */
    void DebugMon_Handler(void);
    /* this function handles PendSV exception */
    void PendSV_Handler(void);
    /* this function handles SysTick exception */
    void SysTick_Handler(void);

    void UART3_IRQHandler(void);

#ifdef __cplusplus
}
#endif
