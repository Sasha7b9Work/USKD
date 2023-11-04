// 2023/08/27 13:49:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/Storage.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT/Sender.h"
#include "Modules/DHT22/DHT22.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>


namespace Storage
{
    static Config config;

    static Measurements meas;

    static bool need_send = false;

    static TimeMeterMS meter;

    static void SendIncidence();
}


void Storage::Update()
{
    SendIncidence();

    if (meter.IsFinished())
    {
        bool result = false;

        PackedTime time = HAL_RTC::GetPackedTime(&result);

        if (!result)
        {
            return;
        }

        meter.SetResponseTime(20000);

        need_send = true;

        //---------------------------------------------------------------------------------------------

        DataDHT dataDHT = DHT::GetMeasuremets();

        meas.SetTemperature(dataDHT.temperature);
        meas.SetHumidity(dataDHT.humidity);

        Sender::SendTemperature(dataDHT.temperature);

        Sender::SendHumidity(dataDHT.humidity);

        //---------------------------------------------------------------------------------------------

        meas.SetCharge(HAL_ADC::GetCharge());

        Sender::SendCharge(meas.GetCharge());

        //---------------------------------------------------------------------------------------------

        Sender::SendDataGPRS(DataGPRS(meas.GetLatitude(), meas.GetLongitude()));

        //---------------------------------------------------------------------------------------------

        time = HAL_RTC::GetPackedTime();

        meas.SetTimeSecs(time.ToSecs());
    }
}


void Storage::SendIncidence()
{
//    static DataMPU6050 max_data;
//
//    DataMPU6050 data = MPU6050::ReadData();
//
//    if (data.CalculateHit() > max_data.CalculateHit())
//    {
//        max_data = data;
////        LOG_WRITE("                                                                    %f g", max_data.CalculateHit());
//    }

    char message[1024] = { 0 };

    char buffer[64];

//    if (max_data.CalculateHit() > 0.0f)
//    {
//        std::sprintf(buffer, "Внешнее воздействие %f, ", max_data.CalculateHit());
//        std::strcat(message, buffer);
//    }

    if (meas.GetTemperature() < config.temperature.min || meas.GetTemperature() > config.temperature.max)
    {
        std::sprintf(buffer, "Температура %.2f, ", meas.GetTemperature());
        std::strcat(message, buffer);
    }

    if (meas.GetHumidity() < config.humidiy.min || meas.GetHumidity() > config.humidiy.max)
    {
        std::sprintf(buffer, "Влажность %.2f", meas.GetHumidity());
        std::strcat(message, buffer);
    }

    if (message[0])
    {
        static TimeMeterMS meter_alerts;

        if (meter_alerts.ElapsedTime() > 3 * 60 * 1000)
        {
            meter_alerts.Reset();

//            Swagger::SendIncident(message);

//            max_data = DataMPU6050();
        }
    }
}


void Storage::SetCoordinates(const DataGPRS &dataGPRS)
{
    meas.SetLatitude(dataGPRS.latitude);
    meas.SetLongitude(dataGPRS.longitude);
}


bool Storage::GetMeasurements(Measurements &measurements)
{
    bool result = need_send;

    measurements = meas;

    return result;
}


void Storage::SetConfig(const Config &_config)
{
    config = _config;

    if (config.report_time == 180)
    {
        config.report_time = 20;
    }

    if (config.check_time == 60)
    {
        config.check_time = 10;
    }

    config.report_time *= 1000;
    config.check_time *= 1000;

//    if (config.temperature.max == 10)
//    {
//        config.temperature.max = 30;
//    }
}
