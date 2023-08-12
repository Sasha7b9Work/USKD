// 2023/04/05 23:31:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct PinUSART_TX
{
    PinUSART_TX() : port(0), pin(0) { }
    void _Init(uint port, uint pin);
private:
    uint port;
    uint pin;
};


struct PinUSART_RX
{
    PinUSART_RX() : port(0), pin(0) { }
    void _Init(uint port, uint pin);
private:
    uint port;
    uint pin;
};


struct PinOUT
{
    PinOUT() : port(0), pin(0) { }
    void _Init(uint port, uint pin);
    void Set();
    void Reset();
    void SetState(bool);
private:
    uint port;
    uint pin;
};


struct PinIN
{
    PinIN() : port(0), pin(0) { }
    void _Init(uint port, uint pin, uint input_mode);
    bool IsLow();
    bool IsHi();
    void DeInit();
private:
    uint port;
    uint pin;
};


extern const uint USART_GPRS_ADDR;
extern PinUSART_TX pinUSART_GPRS_TX;
extern PinUSART_RX pinUSART_GPRS_RX;

extern const uint USART_LOG_ADDR;
extern PinUSART_TX pinUSART_LOG_TX;
extern PinUSART_RX pinUSART_LOG_RX;

extern PinOUT pinGSM_PWR;
extern PinOUT pinGSM_PWRKEY;
extern PinIN  pinGSM_STATUS;
