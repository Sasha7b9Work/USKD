#pragma once


struct DataDHT
{
    DataDHT(float t, float h) : temperature(t), humidity(h) { }
    float temperature;
    float humidity;
};


namespace DHT
{
    DataDHT GetMeasuremets();
}
