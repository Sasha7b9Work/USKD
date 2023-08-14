// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define LOG_WRITE(...)
#define LOG_FUNC_ENTER()
#define LOG_ERROR(...)


#ifdef SOFTWARE_LOG
    extern char log_buffer[8192];
#endif


namespace Log
{
    void Init();

    void Write(char *format, ...);

    void ReceiveFromSIM800(char symbol);
};
