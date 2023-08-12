// 2023/03/30 10:55:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/SSD1306.h"
#include "Display/Font/Font.h"
#include "Hardware/HAL/HAL.h"
#include "Display/Display.h"
#include <cstring>
#include <cstdlib>


namespace SSD1306
{
//    static const uint8 COMMAND        = 0x00;   // Continuation bit=1, D/C=0; 1000 0000
//    static const uint8 DATA           = 0x40;   // Continuation bit=1, D/C=1; 1100 0000

    static bool SendCommand(uint8);
}


void SSD1306::Init()
{
    if (!SendCommand(0xAE)) // Set display OFF
    {
        return;
    }

    SendCommand(0xD4); // Set Display Clock Divide Ratio / OSC Frequency
    SendCommand(0x80); // Display Clock Divide Ratio / OSC Frequency 

    SendCommand(0xA8); // Set Multiplex Ratio
    SendCommand(0x3F); // Multiplex Ratio for 128x64 (64-1)

    SendCommand(0xD3); // Set Display Offset
    SendCommand(0x00); // Display Offset

    SendCommand(0x40); // Set Display Start Line

    SendCommand(0x8D); // Set Charge Pump
    SendCommand(0x14); // Charge Pump (0x10 External, 0x14 Internal DC/DC)

    SendCommand(0xA1); // Set Segment Re-Map
    SendCommand(0xC8); // Set Com Output Scan Direction

    SendCommand(0xDA); // Set COM Hardware Configuration
    SendCommand(0x12); // COM Hardware Configuration

    SendCommand(0x81); // Set Contrast
    SendCommand(0xCF); // Contrast

    SendCommand(0xD9); // Set Pre-Charge Period
    SendCommand(0xF1); // Set Pre-Charge Period (0x22 External, 0xF1 Internal)

    SendCommand(0xDB); // Set VCOMH Deselect Level
    SendCommand(0x40); // VCOMH Deselect Level

    SendCommand(0xA4); // Set all pixels OFF
    SendCommand(0xA6); // Set display not inverted
    SendCommand(0xAF); // Set display On
}


void SSD1306::WriteBuffer(uint8 /*buffer*/[1024])
{
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
    for (uint8 i = 0; i < Display::HEIGHT / 8; i++)
    {
        if (!SendCommand((uint8)(0xB0 + i))) // Set the current RAM page address.
        {
            break;
        }
        SendCommand((uint8)(0x00));
        SendCommand((uint8)(0x10));

//        if (!HAL_I2C::Write(DATA, &buffer[Display::WIDTH * i], Display::WIDTH))
//        {
//            break;
//        }
    };
}


bool SSD1306::SendCommand(uint8 /*command*/)
{
//    return HAL_I2C::Write(COMMAND, &command, 1);
    return true;
}
