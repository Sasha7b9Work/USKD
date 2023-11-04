// 2023/08/27 13:46:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/SIM868.h"


struct ConfigField
{
    ConfigField() : min(0.0f), max(0.0f), deviation(0.0f) { }
    float min;
    float max;
    float deviation;
};


struct Config
{
    Config() : report_time(0), check_time(0) { }
    uint report_time;
    uint check_time;
    ConfigField temperature;
    ConfigField humidiy;
};


struct Measurements
{
    Measurements(uint _time = 0, float _charge = 0.0f, float _latitude = 0.0f, float _longitude = 0.0f, float _temperature = 0.0f, float _humidity = 0.0f, float _radiosity = 0.0f) :
        time_secs(_time),
        latitude(_latitude),
        longitude(_longitude),
        radiosity(_radiosity),
        charge((uint8)(_charge + 0.5f)),
        temperature((uint8)(_temperature + 0.5f)),
        humidity((uint8)(_humidity + 0.5f))
    {
    }

    void SetTimeSecs(uint _time) { time_secs = _time; }
    uint GetTimeSecs() const { return time_secs; }
    void SetTemperature(float _temperature) { temperature = (uint8)(_temperature + 0.5f); }
    float GetTemperature() const { return temperature; }
    void SetHumidity(float _humidity) { humidity = (uint8)(_humidity + 0.5f); }
    float GetHumidity() const { return humidity; }
    void SetCharge(float _charge) { charge = (uint8)(_charge + 0.5f); }
    float GetCharge() const { return charge; }
    void SetLatitude(float _latitude) { latitude = _latitude; }
    float GetLatitude() const { return latitude; }
    void SetLongitude(float _longitude) { longitude = _longitude; }
    float GetLongitude() const { return longitude; }

private:
    uint  time_secs;    // Время от 2000-го года в секундах. Если в старшем бите 1 - это данные для инцидента
    float latitude;     // широта
    float longitude;    // долгота
    float radiosity;    // радиоактивный фон
    uint8 charge;       // заряд батареи
    uint8 temperature;  // температура
    uint8 humidity;     // влажость
};


namespace Storage
{
    void Update();

    void SetCoordinates(const DataGPRS &);

    bool GetMeasurements(Measurements &);

    void SetConfig(const Config &);
}
