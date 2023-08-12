// 2023/04/05 23:31:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace HAL_PINS
{
    void Init();

    void Update();

    void SendState();
}


struct PinADC
{
    PinADC(uint _port, uint _pin, uint8 _channel) : port(_port), pin(_pin), channel(_channel) { }
    void _Init();
    uint8 Channel() const { return channel; }
private:
    uint port;
    uint pin;
    uint8 channel;
};


struct PinI2C
{
    PinI2C() : port(0), pin(0) { }
    void _Init(uint port, uint pin);
private:
    uint port;
    uint pin;
};


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


extern PinADC pinVolt1;     // ACVOLTOUT1
extern PinADC pinVolt2;     // ACVOLTOUT2
extern PinADC pinVolt3;     // ACVOLTOUT3

extern PinADC pinCur1L;     // ACCURROUTL1
extern PinADC pinCur2L;     // ACCURROUTL2
extern PinADC pinCur3L;     // ACCURROUTL3

extern PinADC pinCur1H;     // ACCURROUTH1
extern PinADC pinCur2H;     // ACCURROUTH2
extern PinADC pinCur3H;     // ACCURROUTH3

extern const uint I2C_ADDR;
extern PinI2C pinI2C_SCL;
extern PinI2C pinI2C_SDA;

extern const uint USART_GPRS_ADDR;
extern PinUSART_TX pinUSART_GPRS_TX;
extern PinUSART_RX pinUSART_GPRS_RX;

extern const uint USART_LOG_ADDR;
extern PinUSART_TX pinUSART_LOG_TX;
extern PinUSART_RX pinUSART_LOG_RX;

extern PinOUT pinGSM_PWR;
extern PinOUT pinGSM_PWRKEY;
extern PinIN  pinGSM_STATUS;

extern PinOUT pinMX0;   // \ 
extern PinOUT pinMX1;   // |
extern PinOUT pinMX2;   // | Выбор релея для опроса
extern PinOUT pinMX3;   // |
extern PinOUT pinMX4;   // /
extern PinIN pinP1;     // \ Чтение состояния реле 
extern PinIN pinP2;     // /

extern PinOUT pinKMA1;
extern PinOUT pinKMA2;
extern PinOUT pinKMA3;
extern PinOUT pinKMA4;
extern PinOUT pinKMA5;
extern PinOUT pinKMA6;
extern PinOUT pinKMA7;
extern PinOUT pinKMA8;
extern PinOUT pinKMA9;
extern PinOUT pinKMB1;
extern PinOUT pinKMB2;
extern PinOUT pinKMB3;
extern PinOUT pinKMB4;
extern PinOUT pinKMB5;
extern PinOUT pinKMB6;
extern PinOUT pinKMB7;
extern PinOUT pinKMB8;
extern PinOUT pinKMB9;
extern PinOUT pinKMC1;
extern PinOUT pinKMC2;
extern PinOUT pinKMC3;
extern PinOUT pinKMC4;
extern PinOUT pinKMC5;
extern PinOUT pinKMC6;
extern PinOUT pinKMC7;
extern PinOUT pinKMC8;
extern PinOUT pinKMC9;

extern PinIN pinLevel4;
