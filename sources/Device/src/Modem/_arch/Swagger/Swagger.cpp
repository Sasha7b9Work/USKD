// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Parser.h"
#include "Modem/Swagger/Swagger.h"
#include "Modem/SIM868.h"
#include "Utils/Math.h"
#include "Modem/Swagger/Messages.h"
#include "Modem/Swagger/Route.h"
#include <gd32f30x.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>


/*
*   *** FTP-сервер ***
*   s92153gg.beget.tech/files
*   s92153gg_1
*   Qwerty123!
*/


/*
AT+CREG?
AT+CIPSTATUS
+INITIAL
AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"
AT+SAPBR=3,1,\"APN\",\"internet\"
AT+SAPBR=3,1,\"USER\",\"\"
AT+SAPBR=3,1,\"PWD\",\"\"
AT+SAPBR=1,1
*/


using namespace Parser;
using namespace std;


namespace Swagger
{
    namespace Queue
    {
        static const int MAX_REQUESTS = 20;
        static int num_requests = 0;
        static Request *requests[MAX_REQUESTS];
        static Request *current = nullptr;

        void Append(Request *request)
        {
            if (num_requests < MAX_REQUESTS)
            {
                requests[num_requests++] = request;
            }
        }

        Request *Get()
        {
            Request *result = nullptr;

            if (num_requests)
            {
                result = requests[0];

                for (int i = 1; i < num_requests; i++)
                {
                    requests[i - 1] = requests[i];
                }

                num_requests--;
            }

            return result;
        }

        // true, если в очереди запрос type
        bool InWait(TypeRequest::E type)
        {
            for (int i = 0; i < num_requests; i++)
            {
                if (requests[i]->Type() == type)
                {
                    return true;
                }
            }

            if (current && current->Type() == type)
            {
                return true;
            }

            return false;
        }
    }

    struct State
    {
        enum E
        {
            IDLE,
            RUNNING
        };
    };

    State::E state = State::IDLE;
}


void Swagger::Update(pchar answer)
{
    switch (state)
    {
    case State::IDLE:

        if (Queue::current == nullptr)
        {
            Queue::current = Queue::Get();
        }

        if (Queue::current)
        {
            state = State::RUNNING;
        }

        break;

    case State::RUNNING:

        Queue::current->Update(answer);

        if (Queue::current->IsFinished())
        {
            delete Queue::current;

            Queue::current = nullptr;

            state = State::IDLE;
        }

        break;
    }
}


void Swagger::SendMeasuremets(const Measurements &meas)
{
    if (Route::InRoute() && !Queue::InWait(TypeRequest::_RouteFinish))
    {
        Queue::Append(new Message::SendRecords(meas));
    }
}


void Swagger::SendIncident(pchar incident)
{
    if (Route::InRoute() && !Queue::InWait(TypeRequest::_RouteFinish))
    {
        Queue::Append(new Message::AddIncident(incident));
    }
}


void Swagger::GetServerTime()
{
    Queue::Append(new Message::GetServerTime());
}


void Swagger::GetConfig()
{
    Queue::Append(new Message::GetConfig());
}


void Swagger::StartRoute()
{
    if (!Queue::InWait(TypeRequest::_RouteStart))
    {
        GetServerTime();

        Queue::Append(new Message::RouteStart());
    }
}


void Swagger::StopRoute()
{
    if (!Queue::InWait(TypeRequest::_RouteFinish))
    {
        Queue::Append(new Message::RouteFinish());
    }
}
