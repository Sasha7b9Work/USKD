// 2023/08/15 18:48:36 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage.h"
#include "Hardware/Timer.h"
#include "Modem/Modem.h"


namespace Storage
{
    static float measures[TypeMeasure::Count] =
    {
        0.0f,
        0.0f,
        0.0f,
        0.0f
    };
}


void Storage::Update()
{
    static TimeMeterMS meter;

    if (meter.IsFinished())
    {
        meter.SetResponseTime(30000);

        Modem::SendMeasuremets(measures);
    }
}


void Storage::Set(TypeMeasure::E type, float value)
{
    measures[type] = value;
}


float Storage::Get(TypeMeasure::E type)
{
    return measures[type];
}
