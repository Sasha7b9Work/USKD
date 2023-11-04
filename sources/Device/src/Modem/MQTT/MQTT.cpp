// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/MQTT.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Modem/SIM868.h"
#include "Modem/MQTT/Sender.h"
#include "Modem/MQTT/RequestMQTT.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>


using namespace std;


namespace MQTT
{
    namespace Queue
    {
        static const int MAX_REQUESTS = 2;
        static int num_requests = 0;
        static RequestMQTT *requests[MAX_REQUESTS];
        static RequestMQTT *current = nullptr;
        static int all_requests = 0;

        void Clear()
        {
            all_requests = 0;

            if (current)
            {
                delete current;
                current = nullptr;
            }

            for (int i = 0; i < num_requests; i++)
            {
                delete requests[i];
            }

            num_requests = 0;
        }

        void Append(RequestMQTT *request)
        {
            LOG_WRITE("Requests in queue %d, all %d", num_requests, all_requests++);

            while (num_requests == MAX_REQUESTS)
            {
                Modem::Update();
            }

            requests[num_requests++] = request;
        }

        RequestMQTT *Get()
        {
            RequestMQTT *result = nullptr;

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
        bool InWait(RequestMQTT::Type::E type)
        {
            for (int i = 0; i < num_requests; i++)
            {
                if (requests[i]->GetType() == type)
                {
                    return true;
                }
            }

            if (current && current->GetType() == type)
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

    static bool is_connected = false;

    bool IsConnected()
    {
        return is_connected;
    }

    static State::E state = State::IDLE;
}


void MQTT::Update(pchar answer)
{
    switch (state)
    {
    case State::IDLE:

        if (!is_connected && !Queue::current)
        {
            Queue::Append(new RequestCONNECT(String("-uskd----%s", HAL::GetUID()), "mqttusr", "tgsdj9e5dc"));

            Queue::current = Queue::Get();
        }
        else
        {
            if (Queue::current == nullptr)
            {
                Queue::current = Queue::Get();
            }

            if (Queue::current)
            {
                state = State::RUNNING;
            }
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


void MQTT::Send(RequestMQTT *request)
{
    if (is_connected)
    {
        Queue::Append(request);
    }
    else
    {
        delete request;
    }
}


void MQTT::Callback::OnConnect(bool connect)
{
    is_connected = connect;

    Send(new RequestSUBSCRIBE(String("devices/box/%s/config/getTime", HAL::GetUID())));
}


void MQTT::Reset()
{
    is_connected = false;
    state = State::IDLE;
    Queue::Clear();
}
