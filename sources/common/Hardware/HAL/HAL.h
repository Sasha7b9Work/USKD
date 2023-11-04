// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL_PINS.h"
#include "Utils/String.h"
#include <ctime>


struct PackedTime
{
    uint year    : 6;       // Год после 2000-го
    uint month   : 4;
    uint day     : 5;
    uint hours   : 5;
    uint minutes : 6;
    uint seconds : 6;
    PackedTime(uint h = 11, uint m = 11, uint s = 11, uint d = 11, uint mo = 11, uint y = 11) :
        year(y), month(mo), day(d), hours(h), minutes(m), seconds(s) {};

    uint ToSecs() const;      // Переводит в секунды после 2000-го года

    String ToString() const;

    static PackedTime FromSecs(uint);       // Из секунд после 1 января 2000-го года

    static PackedTime FromTime(const std::tm &time);
};


// Для дисплея
class HAL_I2C
{
public:
    static const uint PERIPH_DISPLAY;
    static const uint PERIPH_MPU6050;

    void Init(uint port, uint pinSCL, uint pinSDA, uint periph, uint adddress);

    bool Write(uint8 reg, uint8 *data, int size);
    bool Read(uint8 reg, uint8 *buf, uint16 len);

private:

    static const uint TIMEOUT = 10;

    PinI2C scl;
    PinI2C sda;
    uint periph;
    uint address;

    bool WaitFlagYes(/*i2c_flag_enum*/ uint);
    bool WaitFlagNo(/*i2c_flag_enum*/ uint);
};


extern HAL_I2C i2cDisplay;


namespace HAL
{
    void Init();
    void DeInit();
    void ErrorHandler();
    pchar GetUID();

    // Если true - находмся на макете
    bool IsLayout();
}


namespace HAL_RTC
{
    void SetDateTime(pchar string);

    PackedTime GetPackedTime(bool *result = nullptr);

    bool TimeIsExist();
}


namespace HAL_ADC
{
    void Init();
    float GetCharge();
}


namespace HAL_USART_GPRS
{
    void Init();
    void DeInit();
    // Передать с завершающм 0x0d
    void Transmit(pchar);
    void Transmit(void *, int);
    void CallbackOnReceive(char);
}


namespace HAL_USART_LOG
{
    void Init();

    // Завершается \r\n
    void Transmit(pchar format, ...);

    // Не завершается ничем
    void TransmitRAW(pchar);
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
    void WriteData(uint address, const void *data, int size);

    void WriteUInt(uint address, uint value);

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

    void USART0_IRQHandler(void);

    void EXTI5_9_IRQHandler(void);

#ifdef __cplusplus
}
#endif
