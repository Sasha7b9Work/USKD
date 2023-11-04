// 2023/08/21 14:59:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/Timer.h"
#include "Storage/Storage.h"
#include <cstdlib>
#include <cstring>


struct Token
{
    static const int SIZE = 80;
    char data[SIZE + 1];
};


struct TypeRequest
{
    enum E
    {
        _RouteStart,
        _RouteFinish,
        _GetConfig,
        _GetServerTime,
        _SendRecords,
        _Incident,
        Count
    };
};


class Request
{
protected:

    struct State
    {
        enum E
        {
            IDLE,
            NEED_SAPBR_2_1,
            NEED_HTTPINIT,
            NEED_HTTPPARA_CID_1,
            NEED_HTTPPARA_URL,
            NEED_HTTPPARA_CONTENT,
            NEED_HTTPDATA,                  // Здесь устанавливаем количество засылаемых байт
            NEED_SEND_DATA,                 // А здесь посылаем данные
            NEED_CONFIRM_SEND_DATA,         // Ждём подтверждения приёма дынных
            NEED_CONFIRM_HTTPACTION_1_OK,   // Ждём ОК от HTTPACTION=1
            NEED_CONFIRM_HTTPACTION_1_FULL, // Ждём +HTTPACTION от HTTPACTION=1
            NEED_HTTPTERM,
            NEED_HTTPREAD_DATA,
            NEED_READ_RAW_DATA,
            FINISHED,
            Count
        };

        State(E v) : value(v) { }

        E value;

        pchar Name() const;
    };

public:

    Request(TypeRequest::E);

    virtual ~Request();

    void Update(pchar);

    pchar Name() const;

    TypeRequest::E Type() const { return type; }

    bool IsFinished() const { return state == State::FINISHED; }

protected:

    static const uint DEFAULT_TIME = 10000;

    TimeMeterMS meter_wait_event;   // Таймер ожидания события

    State::E state;

    bool good_answer;               // Если false - не пришёл ответ 200 от сервера

    char *head;

    char *body;

    TypeRequest::E type;

    void ClearHead();

    void ClearBody();

    virtual void MakeHead() = 0;

    virtual void MakeBody() = 0;

    virtual void ParseAnswer(pchar) = 0;

    void SetState(State::E);

    bool MeterIsRunning(uint time);

    // Ожидать ответ need,
    // затем установить состояние state,
    // и послать сообщение msg
    void WaitSetSend(pchar answer, pchar need, State::E state, pchar msg = nullptr, uint time = DEFAULT_TIME);

private:

    int prev_state;
};


namespace Message
{
    //------------------------------------------------------------------------------------------------- Route Start
    class RouteStart : public Request
    {
    public:

        RouteStart() : Request(TypeRequest::_RouteStart)
        {
        }

        static Token token;

    protected:

        virtual void MakeHead();            // override
        virtual void MakeBody();            // override
        virtual void ParseAnswer(pchar);    // override
    };


    //------------------------------------------------------------------------------------------------- Route Finish
    class RouteFinish : public Request
    {
    public:

        RouteFinish() : Request(TypeRequest::_RouteFinish) { }

    protected:

        virtual void MakeHead();            // override
        virtual void MakeBody();            // override
        virtual void ParseAnswer(pchar);    // override
    };


    //------------------------------------------------------------------------------------------------- Get Config
    class GetConfig : public Request
    {
    public:

        GetConfig() : Request(TypeRequest::_GetConfig) { }

    protected:

        virtual void MakeHead();            // override
        virtual void MakeBody();            // override
        virtual void ParseAnswer(pchar);    // override
    };


    //------------------------------------------------------------------------------------------------- Get Server Time
    class GetServerTime : public Request
    {
    public:

        GetServerTime() : Request(TypeRequest::_GetServerTime) { }

    protected:

        virtual void MakeHead();            // override
        virtual void MakeBody();            // override
        virtual void ParseAnswer(pchar);    // override
    };


    //------------------------------------------------------------------------------------------------- Send Records
    class SendRecords : public Request
    {
    public:

        SendRecords(const Measurements &_meas) :
            Request(TypeRequest::_SendRecords),
            meas(_meas)
        {
        }

        void SendMeasurements(const Measurements &_meas) { meas = _meas; }

    protected:

        virtual void MakeHead();            // override
        virtual void MakeBody();            // override
        virtual void ParseAnswer(pchar);    // override

    private:

        Measurements meas;
    };


    //------------------------------------------------------------------------------------------------- Incident
    class AddIncident : public Request
    {
    public:

        AddIncident(pchar message) : Request(TypeRequest::_Incident)
        {
            buffer = (char *)std::malloc(std::strlen(message) + 1);
            if (buffer)
            {
                std::strcpy(buffer, message);
            }
        }

    protected:

        virtual void MakeHead();            // override
        virtual void MakeBody();            // override
        virtual void ParseAnswer(pchar);    // override

    private:

        char *buffer;
    };
}
