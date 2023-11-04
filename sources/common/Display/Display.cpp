// 2023/03/29 20:05:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Display/SSD1306.h"
#include "Modem/Modem.h"
#include "Modem/MQTT/MQTT.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/SIM868.h"
#include "Utils/String.h"
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

    // Возвращает координату Y строки
    static int Y(int);

    static void WriteFormatFloat(int x, int y, char *format, float value);
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


void Display::Update2()
{
    Fill(0);

    SSD1306::WriteBuffer(buffer);

    Fill(1);

    SSD1306::WriteBuffer(buffer);

    Fill(2);

    SSD1306::WriteBuffer(buffer);
}


void Display::Update()
{
    BeginScene();

    WriteString(70, Y(4), HAL::GetUID());

    Measurements meas;

    if (Storage::GetMeasurements(meas))
    {
        if ((Timer::TimeMS() / 5000) % 2)
        {
            WriteFormatFloat(10, Y(1), "LAT = %f", meas.GetLatitude());

            WriteFormatFloat(10, Y(2), "LON = %f", meas.GetLongitude());
        }
        else
        {
            WriteFormatFloat(10, Y(1), "Temp = %.2f", meas.GetTemperature());

            WriteFormatFloat(10, Y(2), "Hum = %.2f", meas.GetHumidity());

            WriteFormatFloat(10, Y(3), "Bat = %.2f", meas.GetCharge());
        }
    }

    if (Modem::Mode::Power())
    {
        WriteString(5, Y(0), "POW");
    }

    if (SIM868::IsRegistered())
    {
        WriteString(35, Y(0), "REG");
    }

    if (MQTT::IsConnected())
    {
        WriteString(65, Y(0), "MQTT");

        WriteString(100, Y(0), SIM868::LevelSignal());
    }

    SSD1306::WriteBuffer(buffer);
}


void Display::WriteFormatFloat(int x, int y, char *format, float value)
{
    WriteString(x, y, String(format, value));
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


int Display::Y(int row)
{
    return (int)(64.0f / 5.0f * (float)row);
}
