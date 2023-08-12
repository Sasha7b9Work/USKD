// 2023/7/3 14:43:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define DEBUG_POINT Debug::Point(__FILE__, __LINE__)


namespace Debug
{
    void Point(pchar file, int line);
}
