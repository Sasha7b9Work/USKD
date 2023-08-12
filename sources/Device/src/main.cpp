#include "defines.h"
#include "Device.h"


int main()
{
    Device::Init();

    while(1)
    {
        Device::Update();
    }
}
