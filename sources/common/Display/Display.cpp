// 2023/03/29 20:05:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Display/SSD1306.h"
#include "Measurer/Measurer.h"
#include "Modem/Modem.h"
#include "Modem/MQTT/MQTT.h"
#include "Modem/SIM800.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "Display/Font/font10_7.inc"


namespace Display
{
    static const uint SIZE_BUFFER = 64 * 128 / 8;
    static uint8 buffer[SIZE_BUFFER];

    static int cursorX = 0;
    static int cursorY = 0;

    static FontDef font_10x7 = { 7, 10, Font10x7 };

    static void WriteVoltage(int i);

    static void WriteAmpere(int i);
}


namespace Font
{
    extern TypeFont::E type;
}


void Display::Init()
{
    SSD1306::Init();

    Fill(1);

    Timer::DelayMS(250);

    Fill(2);

    Timer::DelayMS(250);

    Fill(0);

    Timer::DelayMS(250);
}


void Display::Update()
{
    FullMeasure measure = Measurer::LastMeasure();

    BeginScene();

    for (int i = 0; i < 3; i++)
    {
        WriteVoltage(i);

        WriteAmpere(i);
    }

    char message[32];

    WriteString(70, 51, HAL::GetUID(message));

    std::sprintf(message, "v%d:%d", gset.GetNumberSteps(), gset.GetKoeffCurrent());

    WriteString(5, 51, message);

    if (Modem::Mode::Power())
    {
        WriteString(5, 5, "POW");
    }

    if (SIM800::IsRegistered())
    {
        WriteString(35, 5, "REG");
    }

    if (MQTT::InStateRunning())
    {
        WriteString(65, 5, "MQTT");

        WriteString(100, 5, SIM800::LevelSignal());
    }

    SSD1306::WriteBuffer(buffer);
}


void Display::WriteVoltage(int i)
{
    FullMeasure measure = Measurer::LastMeasure();

    char message[30];

    std::sprintf(message, "%4.1f", measure.measures[i].voltage);

    WriteString(10, 17 + i * 11, message);
}


void Display::WriteAmpere(int i)
{
    FullMeasure measure = Measurer::LastMeasure();

    char message[30];

    std::sprintf(message, "%4.1f", measure.measures[i].current);

    WriteString(80, 17 + i * 11, message);
}


void Display::BeginScene()
{
    std::memset(buffer, 0, SIZE_BUFFER);
}


void Display::Fill(int color)
{
    if (color == 1)
    {
        std::memset(buffer, 0xFF, SIZE_BUFFER);
    }
    else if(color == 0)
    {
        std::memset(buffer, 0x00, SIZE_BUFFER);
    }
    else if (color == 2)
    {
        for (int i = 0; i < SIZE_BUFFER; i++)
        {
            buffer[i] = (uint8)std::rand();
        }
    }

    SSD1306::WriteBuffer(buffer);
}


void Display::DrawPixel(int x, int y, int color)
{
    if (x >= WIDTH || y >= HEIGHT)
    {
        return;
    };

    if (color)
    {
        buffer[x + (y / 8) * WIDTH] |= 1 << (y % 8);
    }
    else
    {
        buffer[x + (y / 8) * WIDTH] &= ~(1 << (y % 8));
    }
}


char Display::WriteString(int x, int y, pchar str)
{
    cursorX = x;
    cursorY = y;

    while (*str)
    {
        if (WriteChar(*str) != *str)
        {
            return *str;
        };

        str++;
    };

    return *str;
}


char Display::WriteChar(char ch)
{
    if (Font::type == TypeFont::_5 || Font::type == TypeFont::_7)
    {
        uint8 symbol = (uint8)ch;

        int height = Font::GetHeight();
        int width = Font::GetWidth(symbol);

        for (int row = 0; row < height; row++)
        {
            for (int col = 0; col < width; col++)
            {
                if (Font::GetBit(symbol, row, col))
                {
                    DrawPixel(cursorX + col, cursorY + row, 1);
                }
            }
        }

        cursorX += width + 1;
    }
    else if (Font::type == TypeFont::_10)
    {
        if (ch < 32 || ch > 126)
        {
            return 0;
        }

        if (WIDTH < (cursorX + font_10x7.width) || HEIGHT < (cursorY + font_10x7.height))
        {
            return 0;
        };

        for (int i = 0; i < font_10x7.height; i++)
        {
            int b = font_10x7.data[(ch - 32) * font_10x7.height + i];

            for (int j = 0; j < font_10x7.width; j++)
            {
                if ((b << j) & 0x8000)
                {
                    DrawPixel(cursorX + j, cursorY + i, 1);
                }
                else
                {
                    DrawPixel(cursorX + j, cursorY + i, 0);
                };
            };
        };

        cursorX += font_10x7.width;
    }

    return ch;
}
