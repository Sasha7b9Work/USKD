// 2023/04/17 14:03:38 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Font/Font.h"


namespace Font
{
    TypeFont::E type = TypeFont::_10;
}


void Font::SetType(TypeFont::E _type)
{
    type = _type;
}


int Font::GetBit(uint8, int, int)
{
    return 0;
}

int Font::GetHeight()
{
    switch (type)
    {
    case TypeFont::_5:      return 5;
    case TypeFont::_7:      return 7;
    case TypeFont::_10:     return 10;
    case TypeFont::Count:
        break;
    }

    return 10;
}


int Font::GetWidth(uint8)
{
    return 0;
}
