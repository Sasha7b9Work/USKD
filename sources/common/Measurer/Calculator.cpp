// 2023/03/16 16:53:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Measurer.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include <cmath>


namespace Calculator
{
    static int CalculatePeriod(const Sample samples[NUM_SAMPLES]);

    struct Averager
    {
        Averager(uint time) : time_average(time)
        {
            Reset();
        }
        FullMeasure Calculate(const FullMeasure &);
    private:
        void Push(const FullMeasure &);
        void Reset();
        const uint time_average;
        FullMeasure sum;            // Здесь накапливаемые данные для следующего измерения
        FullMeasure measure;        // Здесь последнее измеренное значение
        int counter[Phase::Count];
        uint time_ready_measrue;    // В это время нужно делать новое измерение
    };

    static Averager averager5Sec(5000);
    static Averager averager1Min(60000);

    FullMeasure Average5Sec(const FullMeasure &measure)
    {
        return averager5Sec.Calculate(measure);
    }

    FullMeasure Average1Min(const FullMeasure &measure)
    {
        return averager1Min.Calculate(measure);
    }
}


FullMeasure Calculator::Averager::Calculate(const FullMeasure &meas)
{
    Push(meas);

    FullMeasure result = measure;

    if (Timer::TimeMS() >= time_ready_measrue)
    {
        for (int i = 0; i < Phase::Count; i++)
        {
            if (counter[i] != 0)
            {
                measure.measures[i].voltage = sum.measures[i].voltage / (float)counter[i];
                measure.measures[i].current = sum.measures[i].current / (float)counter[i];
            }
            measure.is_good[i] = (counter[i] != 0);
        }

        result = measure;

        Reset();
    }

    return result;
}


void Calculator::Averager::Push(const FullMeasure &meas)
{
    for (int i = 0; i < Phase::Count; i++)
    {
        if (meas.is_good[i])
        {
            sum.measures[i].voltage += meas.measures[i].voltage;
            sum.measures[i].current += meas.measures[i].current;
            counter[i]++;
        }
    }
}


void Calculator::Averager::Reset()
{
    for (int i = 0; i < Phase::Count; i++)
    {
        counter[i] = 0;
        sum.measures[i].voltage = 0.0f;
        sum.measures[i].current = 0.0f;
        measure.is_good[i] = false;
    }

    time_ready_measrue = Timer::TimeMS() + time_average;
}


void PhaseMeasure::Calculate(const Sample samplesVolts[NUM_SAMPLES], const Sample samplesAmpers[NUM_SAMPLES])
{
    int period = Calculator::CalculatePeriod(samplesVolts);

    // Рассчитываем ток

    float currentRMS = 0.0f;

    for (int i = 0; i < period; i++)
    {
        float value = samplesAmpers[i].ToCurrent();

        currentRMS += value * value;
    }

    current = std::sqrtf(currentRMS / (float)period) * (float)gset.GetKoeffCurrent();

    // Рассчитывем напряжение

    float voltsRMS = 0.0f;

    float max = -1e30f;
    float min = 1e30f;

    for (int i = 0; i < period; i++)
    {
        float value = samplesVolts[i].ToVoltage();

        if (value < min)
        {
            min = value;
        }

        if (value > max)
        {
            max = value;
        }

        voltsRMS += value * value;
    }

    voltage = std::sqrtf(voltsRMS / (float)period);

    if (voltage < 5.0f)
    {
        voltage = 0.0f;
    }
}


void PhaseMeasure::CalculateLimits(const Sample samples[NUM_SAMPLES], FullMeasure *result)
{
    float sum = 0.0f;

    float max = 0.0f;

    float min = 1e5f;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        sum += samples[i];

        if (samples[i] > max)
        {
            max = samples[i];
        }

        if (samples[i] < min)
        {
            min = samples[i];
        }
    }

    result->measures[0].voltage = sum / NUM_SAMPLES;
    result->measures[1].voltage = min;
    result->measures[2].voltage = max;
}


int Calculator::CalculatePeriod(const Sample samples[NUM_SAMPLES])
{
    uint sums[NUM_SAMPLES];

    sums[0] = samples[0];

    for (int i = 1; i < NUM_SAMPLES; i++)
    {
        sums[i] = sums[i - 1] + samples[i];
    }

    int min_period = (int)(NUM_SAMPLES - 2.5f * SAMPLES_ON_PERIOD);     // Минимальный рассматриваемый период
    int max_period = (int)(NUM_SAMPLES - 1.0f * SAMPLES_ON_PERIOD - 3);

    uint min_delta = (uint)(-1);
    int period_min_delta = 0;

    for (int period = min_period; period < max_period; period++)      // Рассматриваем периоды от минимального до максимального
    {
        uint min = (uint)(-1);
        uint max = 0;

        for (int start = 0; start < (int)SAMPLES_ON_PERIOD; start++)
        {
            uint sum = sums[start + period] - sums[start];

            if (sum < min) { min = sum; }

            if (sum > max) { max = sum; }
        }

        uint delta = max - min;

        if (delta < min_delta)
        {
            min_delta = delta;
            period_min_delta = period;
        }
    }

    return period_min_delta;
}
