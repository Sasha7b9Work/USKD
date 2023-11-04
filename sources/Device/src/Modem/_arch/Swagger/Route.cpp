// 2023/09/05 21:00:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Swagger/Route.h"
#include "Hardware/Timer.h"


namespace Route
{
    static uint time_start = 0;
    static bool in_route = false;
}


void Route::Start()
{
    in_route = true;

    time_start = Timer::TimeMS();
}


void Route::End()
{
    in_route = false;
}

uint Route::TimeInRouteInMin()
{
    if (!in_route)
    {
        return 0;
    }

    return (Timer::TimeMS() - time_start) / 1000 / 60;
}


bool Route::InRoute()
{
    return in_route;
}
