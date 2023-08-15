// 2023/08/15 21:06:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/HTTP/Request.h"
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


void Request::Configuration()
{
    Write::Begin();
    Write::String("/api/config/get");
}
