// 2023/10/23 16:31:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/ResponseMQTT.h"
#include "Modem/MQTT/MQTT.h"
#include "Modem/Modem.h"


ResponseMQTT::ResponseMQTT(const Buffer<1024> &) :
    is_unknown(false),
    is_valid(false),
    all_bytes(0),
    type(Type::NONE),
    reason_code(0xFF)
{
}


ResponseMQTT::BufferMQTT::BufferMQTT(const Buffer<1024> &buf) :
    Buffer<1024>(), index(0)
{
    Append(buf.DataConst(), buf.Size());
}


uint8 ResponseMQTT::BufferMQTT::ReadUint8(int *len)
{
    if (*len)
    {
        *len = *len - 1;
    }

    return (uint8)buffer[index++];
}


uint16 ResponseMQTT::BufferMQTT::ReadUint16(int *len)
{
    if (*len)
    {
        *len = *len - 2;
    }

    uint16 hi_byte = (uint8)(buffer[index++] << 8);

    return (uint16)(hi_byte | buffer[index++]);
}


char *ResponseMQTT::BufferMQTT::ReadString(char *out, int _sz, int *len)
{
    if (len)
    {
        *len -= _sz;
    }

    char *p = out;

    for (int i = 0; i < _sz; i++)
    {
        *p = buffer[index++];
        p++;
    }

    *p = '\0';

    return out;
}


int ResponseMQTT::BufferMQTT::ReadVarByte(int *len)
{
    if (len)
    {
        *len = *len - 1;
    }

    return buffer[index++];
}


ResponseMQTT::Type::E ResponseMQTT::BufferMQTT::ReadControlPacketType(int *len)
{
    return (ResponseMQTT::Type::E)ReadUint8(len);
}


ResponseMQTT::Property ResponseMQTT::BufferMQTT::ReadProperty(int *len)
{
    Property result;

    result.length = ReadUint8(len);
    result.type = ReadUint8(len);

    return result;
}


ResponseMQTT ResponseMQTT::Create(const Buffer<1024> &bytes)
{
    if (bytes.Size() < 2)
    {
        return ResponseMQTT(bytes);
    }

    BufferMQTT buffer(bytes);

    ResponseMQTT::Type::E type = buffer.ReadControlPacketType(nullptr);

    int rem_length = buffer.ReadVarByte(nullptr);

    uint8 high_nibble = (uint8)(type & 0xF0);

    if (bytes.Size() < rem_length + 2)
    {
        return ResponseMQTT(bytes);
    }

    if (high_nibble == ResponseMQTT::Type::CONNACK)
    {
        return ResponseCONNECT(bytes);
    }
    else if (high_nibble == ResponseMQTT::Type::PUBLISH)
    {
        return ResponsePUBLISH(bytes);
    }
    else if (high_nibble == ResponseMQTT::Type::SUBACK)
    {
        return ResponseCONNECT(bytes);
    }

    LOG_WRITE_TRACE("in buffer %d bytes", bytes.Size());

    return ResponceUNKNOWN(bytes);
}


bool ResponseMQTT::Run(int *size)
{
    uint8 high_nibble = (uint8)(type & 0xF0);

    if (high_nibble == ResponseMQTT::Type::CONNACK)
    {
        return ((ResponseCONNECT *)this)->Run(size);
    }
    else if (high_nibble == ResponseMQTT::Type::SUBACK)
    {
        return ((ResponseSUBSCRIBE *)this)->Run(size);
    }
    else if (high_nibble == ResponseMQTT::Type::PUBLISH)
    {
        return ((ResponsePUBLISH *)this)->Run(size);
    }
    else if (is_unknown)
    {
        return ((ResponceUNKNOWN *)this)->Run(size);
    }

    return false;
}


ResponceUNKNOWN::ResponceUNKNOWN(const Buffer<1024> &_buffer) :
    ResponseMQTT(_buffer)
{
    BufferMQTT buffer(_buffer);

    type = buffer.ReadControlPacketType(nullptr);

    int rem_length = buffer.ReadVarByte(nullptr);

    all_bytes = rem_length + 2;

    is_valid = true;

    is_unknown = true;
}


ResponseCONNECT::ResponseCONNECT(const Buffer<1024> &_buffer) :
    ResponseMQTT(_buffer),
    flags(0)
{
    BufferMQTT buffer(_buffer);

    type = buffer.ReadControlPacketType(nullptr);

    int rem_length = buffer.ReadVarByte(nullptr);

    all_bytes = rem_length + 2;

    if (buffer.Size() >= all_bytes)
    {
        is_valid = true;

        flags = buffer.ReadUint8(&rem_length);

        reason_code = buffer.ReadUint8(&rem_length);

        Property *prop = &properties[0];

        while (rem_length)
        {
            *prop = buffer.ReadProperty(&rem_length);
            prop++;
        }
    }
}


bool ResponseCONNECT::Run(int *size)
{
    if (is_valid)
    {
        *size = all_bytes;

        (reason_code == 0x00) ? MQTT::Update("MQTT OK") : Modem::Reset(__FILE__, __LINE__);
    }

    return is_valid;
}


bool ResponceUNKNOWN::Run(int *size)
{
    LOG_ERROR("Unknown response type %02X size %d", (int)type, all_bytes);

    if (is_valid)
    {
        *size = all_bytes;
    }

    return is_valid;
}


ResponseSUBSCRIBE::ResponseSUBSCRIBE(const Buffer<1024> &_buffer) :
    ResponseMQTT(_buffer),
    pack_id(0)
{
    BufferMQTT buffer(_buffer);

    type = buffer.ReadControlPacketType(nullptr);

    int rem_length = buffer.ReadVarByte(nullptr);

    all_bytes = rem_length + 2;

    if (buffer.Size() >= all_bytes)
    {
        is_valid = true;

        pack_id = buffer.ReadUint16(nullptr);

        uint8 prop_length = buffer.ReadUint8(nullptr);
        (void)prop_length;

        reason_code = buffer.ReadUint8(nullptr);
    }

    for (int i = 0; i < _buffer.Size(); i++)
    {
        LOG_WRITE_TRACE("%02X", _buffer[i]);
    }
}


bool ResponseSUBSCRIBE::Run(int *size)
{
    if (is_valid)
    {
        *size = all_bytes;

        MQTT::Update(reason_code < 0x80 ? "SUBS OK" : "FALSE");
    }

    return is_valid;
}


ResponsePUBLISH::ResponsePUBLISH(const Buffer<1024> &_buffer) :
    ResponseMQTT(_buffer)
{
    BufferMQTT buffer(_buffer);

    type = buffer.ReadControlPacketType(nullptr);

    int rem_length = buffer.ReadVarByte(nullptr);

    all_bytes = rem_length + 2;

    if (buffer.Size() >= all_bytes)
    {
        is_valid = true;

        uint16 topic_length = buffer.ReadUint16(&rem_length);

        char name_topic[64];

        buffer.ReadString(name_topic, topic_length, &rem_length);

        uint8 lenght_prop = buffer.ReadUint8(&rem_length);

        (void)lenght_prop;

        char message[128];

        buffer.ReadString(message, rem_length, &rem_length);

        LOG_WRITE_TRACE("%s : %s", name_topic, message);
    }
}


bool ResponsePUBLISH::Run(int *size)
{
    if (is_valid)
    {
        *size = all_bytes;

        MQTT::Update("PUBLISH OK");
    }

    return is_valid;
}
