// 2023/10/23 16:31:39 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Buffer.h"


class ResponseMQTT
{
public:

    struct Type
    {
        enum E
        {
            NONE    = 0x00,
            CONNACK = 0x20,
            PUBLISH = 0x30,
            SUBACK  = 0x90
        };
    };

protected:

    struct Property
    {
        struct Type
        {
            enum E
            {
                ReceiveMaximum    = 0x21,
                TopicAliasMaximum = 0x22
            };
        };

        uint8 type;
        uint length;
    };

    struct BufferMQTT : public Buffer<1024>
    {
        BufferMQTT(const Buffer<1024> &buf);

        uint8 ReadUint8(int *len);

        uint16 ReadUint16(int *len);

        char *ReadString(char *, int size, int *len);

        int ReadVarByte(int *len);

        ResponseMQTT::Type::E ReadControlPacketType(int *len);

        Property ReadProperty(int *len);

        int index;
    };

public:

    ResponseMQTT(const Buffer<1024> &);

    static ResponseMQTT Create(const Buffer<1024> &);

    bool Run(int *size);

protected:

    bool is_unknown;
    bool is_valid;
    int all_bytes;
    Type::E type;
    uint8 reason_code;
    Property properties[10];
};


class ResponceUNKNOWN : public ResponseMQTT
{
public:
    ResponceUNKNOWN(const Buffer<1024> &);
    bool Run(int *size);
};


class ResponseCONNECT : public ResponseMQTT
{
public:
    ResponseCONNECT(const Buffer<1024> &);
    bool Run(int *size);
private:
    uint8 flags;
};


class ResponseSUBSCRIBE : public ResponseMQTT
{
public:
    ResponseSUBSCRIBE(const Buffer<1024> &);
    bool Run(int *size);
private:
    uint16 pack_id;
};


class ResponsePUBLISH : public ResponseMQTT
{
public:
    ResponsePUBLISH(const Buffer<1024> &);
    bool Run(int *size);
};
