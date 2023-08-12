#pragma once

struct va_list
{
    char *a;
};

#define va_start(x, y) x.a = (char *)y; //-V1059

int vsprintf(char *__restrict, const char *__restrict, struct va_list);

void va_end(va_list);
