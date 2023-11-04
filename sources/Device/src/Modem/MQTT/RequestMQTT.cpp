// 2023/11/02 17:14:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/RequestMQTT.h"
#include "Modem/SIM868.h"
#include "Hardware/Timer.h"
#include "Modem/Modem.h"
#include "Modem/MQTT/MQTT.h"
#include <cstring>
#include <cstdio>


void Dumper::Init(RequestMQTT *_request, int _counter)
{
    counter = _counter;
    request = _request;
}


void Dumper::Update(char *file, int line)
{
    counter--;

    if (counter > 0)
    {
        request->SetState(RequestMQTT::State::IDLE);

        request->FailMessage();
    }
    else
    {
        RequestMQTT::FuncDumped(file, line);
    }
}


pchar RequestMQTT::Type::Name(RequestMQTT::Type::E type)
{
    static const pchar names[RequestMQTT::Type::Count] =
    {
        "Conect",
        "Subscribe",
        "Publish"
    };

    return names[type];
}


void RequestMQTT::Update(pchar answer)
{
    static TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        SIM868::Transmit::With0D("AT+CIPSEND");
        meter.Reset();
        state = State::NEED_SEND;
        break;

    case State::NEED_SEND:
        if (meter.ElapsedTime() > 1000)
        {
            dump.Update(__FILE__, __LINE__);
        }
        else if(std::strcmp(answer, ">") == 0)
        {
            Send();
//            LOG_WRITE_TRACE("Call \"Send()\" for request \"%s\"", RequestMQTT::Type::Name(type));
            SIM868::Transmit::UINT8(0x1A);
            meter.SetResponseTime(5000);
            state = State::WAIT_CONFIRM;
        }
        break;

    case State::WAIT_CONFIRM:
        if (meter.IsFinished())
        {
            dump.Update(__FILE__, __LINE__);
        }
        else
        {
            if (ProcessAnswer(answer))
            {
//                LOG_WRITE_TRACE("Call \"ProcessAnswer()\" for request \"%s\"", RequestMQTT::Type::Name(type));
                state = State::FINISHED;
            }
        }
        break;

    case State::FINISHED:
        break;
    }
}


void RequestMQTT::FuncDumped(char *file, int line)
{
    Modem::Reset(file, line);
}


void RequestCONNECT::Send()
{
    char *MQTT_type = "MQTT";


    SIM868::Transmit::UINT8(0x10);   // маркер пакета на установку соединения

    SIM868::Transmit::UINT8((uint8)(std::strlen(MQTT_type) + CID.Size() + user.Size() + password.Size() + 18));  // Размер передаваемых в MQTT данных
    SIM868::Transmit::UINT8(0x00);                                                                   // \ Размер MQTT_type
    SIM868::Transmit::UINT8((uint8)std::strlen(MQTT_type));                                          // /
    SIM868::Transmit::RAW(MQTT_type);

    SIM868::Transmit::UINT8(0x05);  // версия протокола
 //                                +------------------ User Name Flag
 //                                |+----------------- Password Flag
 //                                ||    
 //                                ||    +------------ Clean Start
    uint8 connect_flag = BINARY_U8(11000010);

    SIM868::Transmit::UINT8(connect_flag);      // connect flag
    SIM868::Transmit::UINT8(0x00);              // \ keep alive 
    SIM868::Transmit::UINT8(0x3c);              // /

    SIM868::Transmit::UINT8(0x05);              // property length
    SIM868::Transmit::UINT8(0x11);              // Session Expiry Interval identifier
    SIM868::Transmit::UINT8(0x00);              // \ 
    SIM868::Transmit::UINT8(0x00);              // | Session Expiry Interval (10)
    SIM868::Transmit::UINT8(0x01);              // |
    SIM868::Transmit::UINT8(0x2c);              // /

    SIM868::Transmit::UINT8(0x00);
    SIM868::Transmit::UINT8((uint8)CID.Size());
    SIM868::Transmit::RAW(CID);

    SIM868::Transmit::UINT8(0x00);
    SIM868::Transmit::UINT8((uint8)user.Size());
    SIM868::Transmit::RAW(user);

    SIM868::Transmit::UINT8(0x00);
    SIM868::Transmit::UINT8((uint8)password.Size());
    SIM868::Transmit::RAW(password);
}


bool RequestCONNECT::ProcessAnswer(pchar answer)
{
    if (std::strcmp(answer, "MQTT OK") == 0)
    {
        MQTT::Callback::OnConnect(true);

        LOG_WRITE_TRACE("Connect MQTT %s %s %s is OK", CID.c_str(), user.c_str(), password.c_str());

        return true;
    }

    return false;
}


void RequestCONNECT::FailMessage()
{
    LOG_ERROR("Can not connect MQTT %s %s %s", CID.c_str(), user.c_str(), password.c_str());
}


void RequestSUBSCRIBE::Send()
{
    SIM868::Transmit::UINT8(0x82);                  // Control Packet Type
    SIM868::Transmit::UINT8((uint8)(3 + 2 + topic.Size() + 1));
    SIM868::Transmit::UINT8(0x00);                  // \ Packet Identifier 
    SIM868::Transmit::UINT8(0x01);                  // /
    SIM868::Transmit::UINT8(0x00);                  // Property Length
    SIM868::Transmit::UINT8(0x00);                  // \ Topic Length 
    SIM868::Transmit::UINT8((uint8)topic.Size());   // /
    SIM868::Transmit::RAW(topic);
    SIM868::Transmit::UINT8(0x00);

    LOG_WRITE_TRACE("Send subscribe for %s", topic.c_str());
}


bool RequestSUBSCRIBE::ProcessAnswer(pchar answer)
{
    if (std::strcmp(answer, "SUBS OK") == 0)
    {
        LOG_WRITE_TRACE("Subscribe on \"%s\" is succefull", topic.c_str());

        return true;
    }

    return false;
}


void RequestSUBSCRIBE::FailMessage()
{
    LOG_ERROR("Subscribe on \"%s\" is fail", topic.c_str());
}


void RequestPUBLISH::Send()
{
    LOG_WRITE_TRACE("PUBLISH %s %s %s", topic.c_str(), message.c_str(), response.c_str());

    if (response.Size())
    {
        // Fixed Header
        SIM868::Transmit::UINT8(0x31);
        SIM868::Transmit::UINT8((uint8)(2 + 1 + 3 + topic.Size() + message.Size() + response.Size()));

        // Variable Header
        SIM868::Transmit::UINT8(0);
        SIM868::Transmit::UINT8((uint8)topic.Size());
        SIM868::Transmit::RAW(topic);

        SIM868::Transmit::UINT8((uint8)(3 + response.Size()));    // Property length

        SIM868::Transmit::UINT8(0x08);                                  // Property "Responce Topic"
        SIM868::Transmit::UINT8(0x00);
        SIM868::Transmit::UINT8((uint8)response.Size());
        SIM868::Transmit::RAW(response);

        // Payload
        SIM868::Transmit::RAW(message);
    }
    else
    {
        // Fixed Header
        SIM868::Transmit::UINT8(0x31);
        SIM868::Transmit::UINT8((uint8)(3 + topic.Size() + message.Size()));

        // Variable Header
        SIM868::Transmit::UINT8(0);
        SIM868::Transmit::UINT8((uint8)topic.Size());
        SIM868::Transmit::RAW(topic);

        SIM868::Transmit::UINT8(0x00);    // Property length

        // Payload
        SIM868::Transmit::RAW(message);
    }
}


bool RequestPUBLISH::ProcessAnswer(pchar answer)
{
    if (std::strcmp(answer, "PUBLISH OK") == 0)
    {
        return true;
    }

    return false;
}


void RequestPUBLISH::FailMessage()
{
    LOG_ERROR("Fail PUBLISH request");
}
