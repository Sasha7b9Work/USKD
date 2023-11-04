// 2023/08/23 10:49:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/StringUtils.h"
#include <cstring>


char *SU::GetWord(pchar string, int pos_start, int pos_end, char buffer[32])
{
    buffer[0] = '\0';

    char *pointer = buffer;

    int i = pos_start;

    for (; i < pos_end; i++)
    {
        if (string[i])
        {
            *pointer = string[i];
            *(pointer + 1) = '\0';
            pointer++;
        }
        else
        {
            break;
        }
    }

    return buffer;
}


pchar SU::FindSubString(pchar string, pchar sub_string, int num)
{
    pchar entry = nullptr;

    for (int i = 0; i <= num; i++)
    {
        entry = std::strstr(string, sub_string);

        if (!entry)
        {
            break;
        }

        string = entry + std::strlen(sub_string); //-V814
    }

    return entry;
}


pchar SU::AfterSubString(pchar string, pchar sub_string, int num)
{
    pchar entry = FindSubString(string, sub_string, num);

    if (entry)
    {
        entry += std::strlen(sub_string);
    }

    return entry;
}
