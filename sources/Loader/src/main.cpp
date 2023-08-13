// 2023/04/05 10:13:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Loader.h"
#include <gd32f30x.h>


int main()
{
    Loader::Init();

    while (true)
    {
        Loader::Update();
    }
}
