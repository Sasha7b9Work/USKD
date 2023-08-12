// 2023/03/30 10:52:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


/*
*   Измеритель параметров
*/


#define TIME_MEASURE 150                                        // В миллисекундах
#define NUM_PERIODS 7.5f                                        // Приблизительно периодов в одном измерении
#define NUM_SAMPLES  2000                                       // Число точек в одном измерении
#define SAMPLES_ON_PERIOD ((float)NUM_SAMPLES / NUM_PERIODS)    // Точек на периодв


#define NUM_PINS_MX 28          // Количество опрашиваемых по MX пинов

struct Phase
{
    enum E
    {
        A,
        B,
        C,
        Count
    };
};

struct Sample
{
    static const uint16 MAX = 4094;
    static const uint16 ZERO = 2047;
    static const uint16 MIN = 0;

    Sample(uint16 _rel = 0) : rel(_rel) {}
    float ToVoltage() const;
    float ToCurrent() const;
    float VoltsInSample() const { return AmplitudeVoltage() / MAX; }
    float AmpersInSample() const { return AmplitudeCurrent() / MAX; }
    float AmplitudeCurrent() const;                 // Размах тока от MIN до MAX
    float AmplitudeVoltage() const;                 // Размах напряжения от MIN до MAX
    void FromVoltage(float);
    void FromCurrent(float);
    operator uint16() const { return rel; }

private:

    uint16 rel;
};


struct FullMeasure;


struct PhaseMeasure
{
    float voltage;
    float current;

    PhaseMeasure() : voltage(0.0f), current(0.0f) { }

    float GetPower() const { return voltage * current; }

    void Calculate(const Sample samplesVolts[NUM_SAMPLES], const Sample samplesAmpers[NUM_SAMPLES]);

    void CalculateLimits(const Sample samples[NUM_SAMPLES], FullMeasure *out);
};


struct FullMeasure
{
    PhaseMeasure measures[Phase::Count];

    // Если == true, то это измерение нельзя использовать
    bool is_good[Phase::Count];
};


namespace Measurer
{
    void Update();

    void AppendMeasures(uint16 adc_values[6]);

    // Не ноль, если буферы заполнены
    bool BuffersFull();

    FullMeasure LastMeasure();

    FullMeasure Measure5Sec();

    FullMeasure Measure1Min();
}
