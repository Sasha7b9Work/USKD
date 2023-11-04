// 2023/11/03 20:08:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


class String
{
public:
    String() : buffer(nullptr)
    {
    }
    ~String()
    {
        Clear();
    }

    void Clear()
    {
        if (buffer)
        {
            delete buffer;
            buffer = nullptr;
        }
    }

    void Set(pchar string)
    {
        if (Size())
        {
            Clear();
        }

        buffer = new char[std::strlen(string) + 1];

        std::strcpy(buffer, string);
    }

    String(char *format, ...) : buffer(nullptr)
    {
        char data[2048];

        std::va_list args;
        va_start(args, format);
        std::vsprintf(data, format, args);
        va_end(args);

        Set(data);
    }

    pchar c_str() const
    {
        return &buffer[0];
    }

    operator pchar() const
    {
        return &buffer[0];
    }

    int Size() const
    {
        return (int)std::strlen(buffer);
    }

private:
    char *buffer;
};
