// 2023/4/30 18:04:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Parser.h"
#include <cstring>
#include <cstdlib>


using namespace std;


namespace Parser
{
    int NumSeparators(pchar, int pos[10]);
}


int Parser::NumberSymbols(pchar string, char symbol)
{
    int result = 0;

    const char *p = string;

    while (*p)
    {
        if (*p == symbol)
        {
            result++;
        }
        p++;
    }

    return result;
}


int Parser::PositionSymbol(pchar string, char symbol, int num)
{
    int size = (int)std::strlen(string);

    for (int i = 0; i < size; i++)
    {
        if (string[i] == symbol)
        {
            num--;

            if (num == 0)
            {
                return i;
            }
        }
    }

    return -1;
}


pchar Parser::GetWord(pchar string, int pos_start, int pos_end, char out[32])
{
    if (pos_end - pos_start <= 0)
    {
        LOG_ERROR("Wrong arguments");

        out[0] = '\0';
    }
    else
    {
        char *p = out;

        for (int i = pos_start + 1; i < pos_end; i++)
        {
            *p++ = string[i];
        }

        *p = '\0';
    }

    return out;
}


pchar Parser::GetWordInQuotes(pchar string, int num, char out[32])
{
    int size = (int)strlen(string);
    const char *buffer = string;

    int quote1 = num * 2;

    int pos_quote1 = -1;
    int pos_quote2 = -2;

    for (int i = 0; i < size; i++)
    {
        if (buffer[i] == '\"')
        {
            if (quote1 == 0)
            {
                pos_quote1 = i;
                break;
            }
            quote1--;
        }
    }

    for (int i = pos_quote1 + 1; i < size; i++)
    {
        if (buffer[i] == '\"')
        {
            pos_quote2 = i;
            break;
        }
    }

    if (pos_quote1 >= 0 && pos_quote2 >= 0)
    {
        char *pointer = out;

        for (int i = pos_quote1 + 1; i < pos_quote2; i++)
        {
            *pointer++ = buffer[i];
        }

        *pointer = '\0';

        return out;
    }

    return "";
}


pchar Parser::GetWord(pchar string, int num, char out[32])
{
    out[0] = '\0';

    int pos_start = 0;
    int pos_end = 0;

    if (num == 1)
    {
        pos_start = -1;
    }
    else
    {
        const char *p = string;

        int current_word = 1;

        bool in_word = (*p != ' ') && (*p != ',') && (*p != ':') && (*p != '=');

        while (current_word < num && *p)
        {
            if (*p == ' ' || *p == ',' || *p == ':' || *p == '=')
            {
                if (in_word)
                {
                    in_word = false;
                }
            }
            else
            {
                if (!in_word)
                {
                    current_word++;
                    in_word = true;
                    if (current_word == num)
                    {
                        pos_start = p - string - 1;
                        break;
                    }
                }
            }

            p++;
        }

        if (current_word != num)
        {
            return "";
        }
    }

    pos_end = pos_start + 1;

    const char *p = string + pos_end;

    while (*p != '\0' && *p != ' ' && *p != ',' && *p != ':' && *p != '=')
    {
        pos_end++;
        p++;
    }

    return GetWord(string, pos_start, pos_end, out);
}


int Parser::NumSeparators(pchar string, int pos[10])
{
    int size = (int)strlen(string);

    const char *p = string;

    int num_sep = 0;

    for (int i = 0; i < size; i++)
    {
        if (*p == ' ' || *p == ',' || *p == ':')
        {
            pos[num_sep++] = i;
        }
        p++;
    }

    pos[num_sep++] = size;

    return num_sep;
}


float Parser::SymbolsToFloat(pchar string, int pos_start, int pos_end)
{
    char buffer[32];

    char *pointer = buffer;

    for (int i = pos_start; i < pos_end; i++)
    {
        *pointer++ = string[i];
    }

    *pointer = '\0';

    return std::atof(buffer);
}
