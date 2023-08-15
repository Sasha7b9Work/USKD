// 2023/08/15 21:04:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/HTTP/Answer.h"
#include <cstring>


namespace Answer
{
    static char buffer[1024];
    static bool exist = false;
}


void Answer::Set(char *data)
{
    std::strcpy(buffer, data);
    exist = true;
}


bool Answer::Exist()
{
    return exist;
}
