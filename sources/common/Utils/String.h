// 2023/11/03 20:08:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


template<int capacity>
class String
{
public:
    String()
    {
        buffer[0] = '\0';
    }
    void Set(pchar string)
    {
        std::strcpy(buffer, string);
    }
    String(char *format, ...)
    {
        std::va_list args;
        va_start(args, format);
        std::vsprintf(buffer, format, args);
        va_end(args);
    }

    pchar c_str() const
    {
        return &buffer[0];
    }

    operator pchar() const
    {
        return &buffer[0];
    }

    operator char *() const
    {
        return &buffer[0];
    }
    int Size() const
    {
        return (int)std::strlen(buffer);
    }

private:
    char buffer[capacity];
};
