// 2023/10/23 10:44:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/ProcessorBinaryData.h"
#include "Utils/Buffer.h"
#include "Modem/MQTT/ResponseMQTT.h"


namespace PBD
{
    static Buffer<1024> buffer;

    static void Process();
}


void PBD::AppendByte(uint8 byte)
{
    buffer.Append((char)byte);

    if (buffer.Size())
    {
        Process();
    }
}


void PBD::Process()
{
    ResponseMQTT responseMQTT = ResponseMQTT::Create(buffer);

    int removed_bytes = 0;

    if (responseMQTT.Run(&removed_bytes))
    {
        buffer.RemoveFirst(removed_bytes);
    }
}
