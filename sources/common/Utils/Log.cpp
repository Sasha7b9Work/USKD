// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


using namespace std;


namespace Log
{
    static int counter = 0;
}


void Log::Write(char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    char buffer[300];
    std::sprintf(buffer, "%s\r\n", message);

    HAL_USART_LOG::TransmitRAW(buffer);
}


void Log::WriteTrace(char *file, int line, char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    char buffer[512];

    std::sprintf(buffer, "%d : %s                                  %s:%d", counter++, message, file, line);

    HAL_USART_LOG::Transmit(buffer);
}


void Log::Error(char *file, int line, char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    char buffer[300];

    std::sprintf(buffer, "                !!! ERROR !!! %s : %s:%d\r\n", message, file, line);

    HAL_USART_LOG::TransmitRAW(buffer);
}
