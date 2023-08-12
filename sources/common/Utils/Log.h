// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#ifdef ENABLE_LOG
#define LOG_WRITE(...)      Log::Write(__VA_ARGS__)
#define LOG_FUNC_ENTER()    Log::Write("Enter:%s:%d", __FUNCTION__, __LINE__)
#define LOG_ERROR(...)      Log::Write(__VA_ARGS__)
#else
#define LOG_WRITE(...)
#define LOG_FUNC_ENTER()
#define LOG_ERROR(...)
#endif


#ifdef SOFTWARE_LOG
    extern char log_buffer[8192];
#endif


namespace Log
{
    void Init();

    void Write(char *format, ...);

    void ReceiveFromSIM800(char symbol);
};
