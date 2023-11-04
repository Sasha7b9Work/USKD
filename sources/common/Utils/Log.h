// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL.h"


#define LOG_WRITE(...)
#define LOG_WRITE_TRACE(...)
#define LOG_FUNC_ENTER()
#define LOG_ERROR(...)


// #define LOG_WRITE(...)          HAL_USART_LOG::Transmit(__VA_ARGS__)
// #define LOG_WRITE_TRACE(...)    Log::WriteTrace(__FILE__, __LINE__, __VA_ARGS__)
// #define LOG_FUNC_ENTER()
// #define LOG_ERROR(...)          Log::Error(__FILE__, __LINE__, __VA_ARGS__)


namespace Log
{
    void Write(char *format, ...);

    void WriteTrace(char *file, int line, char *format, ...);

    void Error(char *file, int line, char *format, ...);
};
