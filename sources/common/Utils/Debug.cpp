// 2023/7/3 14:42:36 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Debug.h"


void Debug::Point(pchar /*file*/, int /*line*/)
{
    LOG_WRITE("%s : %d", file, line);
}
