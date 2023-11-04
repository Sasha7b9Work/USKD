// 2023/04/05 23:31:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct PinADC
{
    PinADC(uint _port, uint _pin, uint8 _channel) :
        port(_port), pin(_pin), channel(_channel)
    { }
    void Init();
    uint8 Channel() const { return channel; }
private:
    uint port;
    uint pin;
    uint8 channel;
};


struct PinUSART_TX
{
    PinUSART_TX(uint _port, uint _pin) : port(_port), pin(_pin) { }
    void Init();
private:
    uint port;
    uint pin;
};


struct PinUSART_RX
{
    PinUSART_RX(uint _port, uint _pin) : port(_port), pin(_pin) { }
    void Init();
private:
    uint port;
    uint pin;
};


struct PinOUT
{
    PinOUT(uint _port, uint _pin) : port(_port), pin(_pin) { }
    void Init();
    void Set();
    void Reset();
    void SetState(bool);
private:
    uint port;
    uint pin;
};


struct PinI2C
{
    PinI2C() : port(0), pin(0) { }
    void Init(uint port, uint pin, uint mode);
private:
    uint port;
    uint pin;
};


struct PinIN
{
    PinIN(uint _port, uint _pin, uint _mode) : port(_port), pin(_pin), mode(_mode)  { }
    void Init();
    bool IsLow();
    bool IsHi();
    void DeInit();
private:
    uint port;
    uint pin;
    uint mode;
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

extern PinADC pinBAT;                 // Для измерения напряжения АКБ

extern PinIN  pinGC777;
