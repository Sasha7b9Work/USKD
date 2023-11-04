// 2023/11/02 17:14:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/GlobalFunctions.h"
#include <cstring>


class RequestMQTT;


// Сбрасыватель.
// Каждый Update() уменьшает значение на 1 и когда оно достигнет нуля,
// вызывает функцию
struct Dumper
{
    void Init(RequestMQTT *request, int counter);
    void Update(char *, int);
private:
    int counter;
    RequestMQTT *request;
};


class RequestMQTT
{
    friend struct Dumper;

public:
    struct Type
    {
        enum E
        {
            CONNECT,
            SUBSCRIBE,
            PUBLISH,
            Count
        };

        static pchar Name(E);
    };

    RequestMQTT(Type::E _type) : state(State::IDLE), type(_type)
    {
        dump.Init(this, 5);
    }

    virtual ~RequestMQTT() { };

    void Update(pchar);

    bool IsFinished() const { return state == State::FINISHED; };

    Type::E GetType() const { return type; }

protected:

    struct State
    {
        enum E
        {
            IDLE,
            NEED_SEND,      // Ждёи ">" для передачи сообщения
            WAIT_CONFIRM,   // Ожидаем подтверждения
            FINISHED
        };
    };

    State::E state;

    Type::E type;

    Dumper dump;

    uint time_send;

    // Посылает информацию
    virtual void Send() = 0;

    virtual bool ProcessAnswer(pchar) = 0;

    // Функция вызывается, когда что-то пошло не так
    static void FuncDumped(char *, int);

    void SetState(State::E st) { state = st; }

    virtual void FailMessage() = 0;
};


class RequestCONNECT : public RequestMQTT
{
public:
    RequestCONNECT(pchar _CID, pchar _user, pchar _password) :
        RequestMQTT(RequestMQTT::Type::CONNECT)
    {
        CID.Set(_CID);
        user.Set(_user);
        password.Set(_password);
    }

    virtual ~RequestCONNECT() { }

protected:

    virtual void Send();

    virtual bool ProcessAnswer(pchar);

    virtual void FailMessage();

private:

    String CID;
    String user;
    String password;
};


class RequestSUBSCRIBE : public RequestMQTT
{
public:
    RequestSUBSCRIBE(pchar _topic) :
        RequestMQTT(RequestMQTT::Type::SUBSCRIBE)
    {
        topic.Set(_topic);
    }

    virtual ~RequestSUBSCRIBE() { }

protected:

    virtual void Send();

    virtual bool ProcessAnswer(pchar);

    virtual void FailMessage();

private:

    String topic;
};


class RequestPUBLISH : public RequestMQTT
{
public:
    RequestPUBLISH(pchar _topic, pchar _message, pchar _response) :
        RequestMQTT(RequestMQTT::Type::PUBLISH)
    {
        topic.Set(_topic);
        message.Set(_message);
        response.Set(_response);
    }

    virtual ~RequestPUBLISH() { }

protected:

    virtual void Send();

    virtual bool ProcessAnswer(pchar);

    virtual void FailMessage();

private:

    String topic;
    String message;
    String response;
};
