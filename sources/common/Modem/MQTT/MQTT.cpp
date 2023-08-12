// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/MQTT.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Modem/SIM868.h"
#include "Modem/MQTT/Sender/Sender.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>


using namespace std;


namespace MQTT
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_RESPONSE_CIPSEND,      // ∆деЄм приглашени€ ">"
            RUNNING
        };
    };

    static State::E state = State::IDLE;

    // —брасываетс€ каждый раз при поступлении данынх
    static TimeMeterMS meterLastData;
}


bool MQTT::InStateRunning()
{
    return state == State::RUNNING;
}


bool MQTT::InStateIdle()
{
    return state == State::IDLE;
}


void MQTT::Reset()
{
    state = State::IDLE;
    meterLastData.Reset();
}


void MQTT::Update(pchar answer)
{
    static TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        Sender::Reset();
        LOG_WRITE("+++ MQTT::IDLE +++");
        SIM868::Transmit::With0D("AT+CIPSEND");
        meter.Reset();
        state = State::WAIT_RESPONSE_CIPSEND;
        break;

    case State::WAIT_RESPONSE_CIPSEND:
        if (strcmp(answer, ">") == 0)
        {
            char *MQTT_type = "MQTT";

            char guid[32];
            char MQTT_CID[32];

            HAL::GetUID(guid);

            std::sprintf(MQTT_CID, "pks-%d%02d-%s", 4, 16, guid);

            LOG_WRITE(MQTT_CID);            // WARNING без этого не соедин€етс€

            SIM868::Transmit::UINT8(0x10);   // маркер пакета на установку соединени€
            SIM868::Transmit::UINT8((uint8)(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + 8));

            // тип протокола
            SIM868::Transmit::UINT8(0x00);
            SIM868::Transmit::UINT8((uint8)std::strlen(MQTT_type));
            SIM868::Transmit::RAW(MQTT_type);

            // просто так нужно
            SIM868::Transmit::UINT8(0x04);    // верси€ протокола
            SIM868::Transmit::UINT8(0x02);    // connect flag
            SIM868::Transmit::UINT8(0x00);    // \ keep alive 
            SIM868::Transmit::UINT8(0x3c);    // /

            SIM868::Transmit::UINT8(0x00);    // property lenth
            SIM868::Transmit::UINT8((uint8)std::strlen(MQTT_CID));
            SIM868::Transmit::RAW(MQTT_CID);

            SIM868::Transmit::UINT8(0x1A);

            state = State::RUNNING;

            meterLastData.Reset();
        }
        break;

    case State::RUNNING:

        Sender::SendAll(answer);

        if (meterLastData.ElapsedTime() > 60000)
        {
            Modem::Reset();
        }

        break;
    }
}


void  MQTT::Packet::Publish(pchar MQTT_topic, pchar MQTT_messege)
{
    SIM868::Transmit::UINT8(0x30);
    SIM868::Transmit::UINT8((uint8)(std::strlen(MQTT_topic) + std::strlen(MQTT_messege) + 2));
    SIM868::Transmit::UINT8(0);
    SIM868::Transmit::UINT8((uint8)(std::strlen(MQTT_topic)));
    SIM868::Transmit::RAW(MQTT_topic);
    SIM868::Transmit::RAW(MQTT_messege);

//    LOG_WRITE("publish %s : %s", MQTT_topic, MQTT_messege);
}


void MQTT::Packet::Publish(pchar topic, int value)
{
    char buffer[32];
    sprintf(buffer, "%d", value);
    Publish(topic, buffer);
}


void MQTT::Packet::PublishF(pchar topic, float value)
{
    char buffer[32];

    if (value < 10.0f)
    {
        sprintf(buffer, "%2.1f", value);
    }
    else
    {
        sprintf(buffer, "%d", (int)(value + 0.5f));
    }

    Publish(topic, buffer);
}


void MQTT::CallbackOnReceiveData(pchar answer)
{
    meterLastData.Reset();

    LOG_WRITE((char *)answer);
}
