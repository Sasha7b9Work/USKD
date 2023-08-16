// 2023/08/15 21:06:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/HTTP/POST/Request.h"
#include <cstring>


namespace Request
{
    static char buffer[1024];

    namespace Write
    {
        static void Begin()
        {
            buffer[0] = '\0';
        }
        static void String(char *string)
        {
            std::strcat(buffer, string);
        }
    }
}


void Request::Set(char *request)
{
    std::strcpy(buffer, request);
}


int Request::Lenght()
{
    return (int)std::strlen(buffer);
}
