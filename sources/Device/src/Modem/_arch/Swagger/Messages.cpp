// 2023/08/22 08:22:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Swagger/Messages.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Modem/SIM868.h"
#include "Modem/Swagger/Swagger.h"
#include "Storage/Storage.h"
#include "Utils/StringUtils.h"
#include "Modem/Swagger/Route.h"
#include <cstdio>
#include <cstring>


using namespace Parser;


namespace Body
{
    static char buffer[1024];

    void Set(char *request)
    {
        std::strcpy(buffer, request);
    }


    char *Get()
    {
        return buffer;
    }


    int Lenght()
    {
        return (int)std::strlen(buffer);
    }
}


Token Message::RouteStart::token;


Request::Request(TypeRequest::E _type) :
    state(State::IDLE),
    head(nullptr),
    body(nullptr),
    type(_type),
    prev_state(-1)
{

}


Request::~Request()
{
    ClearHead();
    ClearBody();
}


void Request::ClearHead()
{
    if (head)
    {
        delete head;

        head = nullptr;
    }
}

void Request::ClearBody()
{
    if (body)
    {
        delete body;

        body = nullptr;
    }
}


void Request::SetState(State::E _state)
{
    state = _state;
    meter_wait_event.Reset();
}


bool Request::MeterIsRunning(uint time)
{
    if (meter_wait_event.ElapsedTime() < time)
    {
        return true;
    }

    SetState(State::IDLE);

    return false;
}


pchar Request::Name() const
{
    static const pchar names[TypeRequest::Count] =
    {
        "Route Start",
        "Route Finish",
        "Get Config",
        "Get Server Time",
        "Send Records",
        "Incident"
    };

    return names[type];
}


void Request::Update(pchar answer)
{
    BufferParser buffer;

    switch (state)
    {
    case State::IDLE:
        ClearHead();
        ClearBody();
        MakeHead();
        MakeBody();
        LOG_WRITE("HEAD : %s", head);
        LOG_WRITE("BODY : %s", body);
        SetState(State::NEED_SAPBR_2_1);
        SIM868::Transmit::With0D("AT+SAPBR=1,1");
        break;

    case State::NEED_SAPBR_2_1: WaitSetSend(answer, "OK", State::NEED_HTTPINIT, "AT+SAPBR=2,1");
        break;

    case State::NEED_HTTPINIT:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (std::strcmp(GetWord(answer, 1, &buffer), "+SAPBR") == 0)
            {
                int first = GetWord(answer, 2, &buffer)[0] & 0x0f;
                int second = GetWord(answer, 3, &buffer)[0] & 0x0f;

                if (first == 1 && second == 1)
                {
                    SetState(State::NEED_HTTPPARA_CID_1);
                    SIM868::Transmit::With0D("AT+HTTPINIT");
                }
            }
        }
        break;

    case State::NEED_HTTPPARA_CID_1:    WaitSetSend(answer, "OK", State::NEED_HTTPPARA_URL, "AT+HTTPPARA=\"CID\",1");
        break;

    case State::NEED_HTTPPARA_URL:      WaitSetSend(answer, "OK", State::NEED_HTTPPARA_CONTENT, head);
        break;

    case State::NEED_HTTPPARA_CONTENT:  WaitSetSend(answer, "OK", State::NEED_HTTPDATA, "AT+HTTPPARA=\"CONTENT\",\"application/json\"");
        break;

    case State::NEED_HTTPDATA:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "OK") == 0)
            {
                Body::Set(body);
                SetState(State::NEED_SEND_DATA);

                char message[32];
                std::sprintf(message, "AT+HTTPDATA=%d,10000", Body::Lenght());
                SIM868::Transmit::With0D(message);
            }
        }
        break;

    case State::NEED_SEND_DATA:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (std::strcmp(answer, "DOWNLOAD") == 0)
            {
                SIM868::Transmit::RAW(Body::Get());
                ClearHead();
                ClearBody();
                SetState(State::NEED_CONFIRM_SEND_DATA);
            }
        }
        break;

    case State::NEED_CONFIRM_SEND_DATA:
        WaitSetSend(answer, "OK", State::NEED_CONFIRM_HTTPACTION_1_OK, "AT+HTTPACTION=1");
        break;

    case State::NEED_CONFIRM_HTTPACTION_1_OK:
        WaitSetSend(answer, "OK", State::NEED_CONFIRM_HTTPACTION_1_FULL);
        break;

    case State::NEED_CONFIRM_HTTPACTION_1_FULL:
        if (MeterIsRunning(5000))
        {
            if (answer[0])
            {
                if (std::strcmp(GetWord(answer, 1, &buffer), "+HTTPACTION") == 0)
                {
                    good_answer = std::strcmp(GetWord(answer, 3, &buffer), "200") == 0;

                    SetState(State::NEED_HTTPTERM);
                    SIM868::Transmit::With0D("AT+HTTPREAD");
                }
            }
        }
        break;

    case State::NEED_HTTPTERM:
        if (MeterIsRunning(5000))
        {
            if (answer[0])
            {
                if (std::strcmp(answer, "OK") == 0)
                {
                    SIM868::Transmit::With0D("AT+HTTPTERM");
                }
                else if (std::strcmp(GetWord(answer, 1, &buffer), "+HTTPREAD") == 0)
                {
                    SetState(State::NEED_READ_RAW_DATA);
                }
                else
                {
                    LOG_WRITE("%d : ANSWER: %s", type, answer);
                }
            }
        }
        break;

    case State::NEED_READ_RAW_DATA:
        if (MeterIsRunning(5000))
        {
            if (answer[0])
            {
                ParseAnswer(answer);

                if (good_answer)
                {
                    LOG_WRITE("%d : ANSWER2: %s", type, answer);

                }
                else
                {
                    LOG_WRITE("\r\n%d : !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Bad ANSWER2: %s", type, answer);
                }

                SIM868::Transmit::With0D("AT+HTTPTERM");
            }
        }
        break;

    case State::NEED_HTTPREAD_DATA:
        break;

    case State::FINISHED:
        break;

    case State::Count:
        break;
    }
}


void Request::WaitSetSend(pchar answer, pchar need, State::E _state, pchar msg, uint time)
{
    if (MeterIsRunning(time))
    {
        if (std::strcmp(answer, need) == 0)
        {
            SetState(_state);

            if (msg)
            {
                SIM868::Transmit::With0D(msg);
            }
        }
    }
}


void Message::RouteStart::MakeHead()
{
    pchar string = "AT+HTTPPARA=\"URL\",\"dev2.smartwrap.tech/api/route/start\"";

    head = new char[std::strlen(string) + 1];

    std::strcpy(head, string);
}


void Message::RouteStart::MakeBody()
{
    // Формат времени - Iso8601

    char uid[32];
    char request[256];
    char time[64];
    std::sprintf(request, "{\"serialNumber\":\"%s\",\"configId\":0,\"time\":\"%s\"}", HAL::GetUID(uid), HAL_RTC::GetPackedTime().ToString(time));

    body = new char[std::strlen(request) + 1];
    std::strcpy(body, request);
}


void Message::RouteStart::ParseAnswer(pchar answer)
{
    if (good_answer)
    {
        BufferParser buffer;

        GetWord(answer, 5, &buffer);

        std::strcpy(token.data, buffer.data + 1); //-V512

        token.data[std::strlen(token.data) - 2] = 0;      // Взяли токен //-V557

        state = State::FINISHED;

        Route::Start();
    }
    else
    {
        state = State::IDLE;
    }
}


void Message::RouteFinish::MakeHead()
{
    pchar string = "AT+HTTPPARA=\"URL\",\"dev2.smartwrap.tech/api/route/stop\"";

    head = new char[std::strlen(string) + 1];

    std::strcpy(head, string);
}


void Message::RouteFinish::MakeBody()
{
    char request[256];
    char time[64];
    std::sprintf(request, "{\"sessionToken\":\"%s\",\"time\":\"%s\"}", Message::RouteStart::token.data, HAL_RTC::GetPackedTime().ToString(time));

    body = new char[std::strlen(request) + 1];
    std::strcpy(body, request);
}


void Message::RouteFinish::ParseAnswer(pchar)
{
    state = State::FINISHED;

    Route::End();
}


void Message::GetConfig::MakeHead()
{
    pchar string = "AT+HTTPPARA=\"URL\",\"dev2.smartwrap.tech/api/config/get\"";

    head = new char[std::strlen(string) + 1];   

    std::strcpy(head, string);
}


void Message::GetConfig::MakeBody()
{
    char uid[32];
    char message[256];
    std::sprintf(message, "{\"serialNumber\":\"%s\"}", HAL::GetUID(uid));

    body = new char[std::strlen(message) + 1];

    std::strcpy(body, message);
}


void Message::GetConfig::ParseAnswer(pchar answer)
{
    pchar entry = SU::AfterSubString(answer, "\"id\":1", 0);

    if (entry)
    {
        entry = SU::AfterSubString(entry, "reportTime\":", 0);

        if (entry)
        {
            Config config;

            BufferParser buffer;

            Parser::GetWord(entry, 1, &buffer);

            config.report_time = buffer.ToUINT();                           // reportTime

            entry = SU::AfterSubString(entry, "checkTime\":", 0);

            if (entry)
            {
                Parser::GetWord(entry, 1, &buffer);

                config.check_time = buffer.ToUINT();                        // checkTime

                entry = Parser::ParseConfigField(entry, "temperatureCondition\":", config.temperature);

                if (entry)
                {
                    entry = Parser::ParseConfigField(entry, "humidityCondition\":", config.humidiy);

                    if (entry)
                    {
                        Storage::SetConfig(config);

                        state = State::FINISHED;

                        return;
                    }
                }
            }
        }
    }

    state = State::IDLE;
}


void Message::GetServerTime::MakeHead()
{
    pchar string = "AT+HTTPPARA=\"URL\",\"dev2.smartwrap.tech/api/config/getTime\"";

    head = new char[std::strlen(string) + 1];

    std::strcpy(head, string);
}


void Message::GetServerTime::MakeBody()
{
    char uid[32];
    char request[256];
    HAL::GetUID(uid);
    std::sprintf(request, "{\"serialNumber\":\"%s\"}", uid);

    body = new char[std::strlen(request) + 1];
    std::strcpy(body, request);
}


void Message::GetServerTime::ParseAnswer(pchar answer)
{
    if (!good_answer)
    {
        SetState(State::IDLE);
    }
    else
    {
        int start = Parser::PositionSymbol(answer, '\"', 3) + 1;

        int end = Parser::PositionSymbol(answer, '\"', 4);

        if (end > 0)
        {
            char buffer[64];

            std::memcpy(buffer, answer + start, (uint)(end - start));
            buffer[end - start + 1] = '\0';
            HAL_RTC::SetDateTime(buffer);

            SetState(State::FINISHED);
        }
        else
        {
            SetState(State::IDLE);
        }
    }
}


void Message::SendRecords::MakeHead()
{
    pchar string = "AT+HTTPPARA=\"URL\",\"dev2.smartwrap.tech/api/route/addRecords\"";

    head = new char[std::strlen(string) + 1];

    std::strcpy(head, string);
}


void Message::SendRecords::MakeBody()
{
    static char message[512];
    static char time[64];

    std::sprintf(message,

        "{\"sessionToken\": \"%s\","
        "\"recordsDataList\": ["
        "{"
        "\"time\": \"%s\","
        "\"latitude\": %f,"
        "\"longitude\": %f,"
        "\"temperature\": %f,"
        "\"charge\": %f,"
        "\"humidity\": %f,"
        "\"otherSensors\": {"
        "\"externalTemperature\": 20,"
        "\"geiger\": 888"
        "}"
        "}"
        "]"
        "}",

        RouteStart::token.data,
        PackedTime::FromSecs(meas.GetTimeSecs()).ToString(time),
        meas.GetLatitude(),
        meas.GetLongitude(),
        meas.GetTemperature(),
        meas.GetCharge(),
        meas.GetHumidity()
    );

    body = new char[std::strlen(message) + 1];

    std::strcpy(body, message);
}


void Message::SendRecords::ParseAnswer(pchar)
{
    state = State::FINISHED;
}


void Message::AddIncident::MakeHead()
{
    pchar string = "AT+HTTPPARA=\"URL\",\"dev2.smartwrap.tech/api/incident/add\"";

    head = new char[std::strlen(string) + 1];

    std::strcpy(head, string);
}


void Message::AddIncident::MakeBody()
{
    static char message[2048];
    char time[64];

    Measurements meas;
    
    Storage::GetMeasurements(meas);

    std::sprintf(message,

        "{\"sessionToken\": \"%s\","
        "\"code\":0,"
        "\"description\":\"%s\","
        "\"time\":\"%s\","
        "\"record\":{"
        "\"time\":\"%s\","
        "\"latitude\":%f,"
        "\"longitude\":%f,"
        "\"temperature\":%f,"
        "\"charge\":%f,"
        "\"humidity\":%f,"
        "\"otherSensors\": {"
        "\"externalTemperature\": 0,"
        "\"geiger\": 0"
        "}"
        "}"
        "}",

        RouteStart::token.data,
        buffer,
        PackedTime::FromSecs(meas.GetTimeSecs()).ToString(time),
        PackedTime::FromSecs(meas.GetTimeSecs()).ToString(time),
        meas.GetLatitude(),
        meas.GetLongitude(),
        meas.GetTemperature(),
        meas.GetCharge(),
        meas.GetHumidity()
    );

    body = new char[std::strlen(message) + 1];

    std::strcpy(body, message);
}


void Message::AddIncident::ParseAnswer(pchar)
{
    std::free(buffer);

    state = State::FINISHED;
}


pchar Request::State::Name() const
{
    static const pchar names[Count] =
    {
        "IDLE",
        "NEED_SAPBR_2_1",
        "NEED_HTTPINIT",
        "NEED_HTTPPARA_CID_1",
        "NEED_HTTPPARA_URL",
        "NEED_HTTPPARA_CONTENT",
        "NEED_HTTPDATA",
        "NEED_SEND_DATA",
        "NEED_CONFIRM_SEND_DATA",
        "NEED_CONFIRM_HTTPACTION_1_OK",
        "NEED_CONFIRM_HTTPACTION_1_FULL",
        "NEED_HTTPTERM",
        "NEED_HTTPREAD_DATA",
        "NEED_READ_RAW_DATA"
    };

    return names[value];
}
