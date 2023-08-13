// 2023/05/21 11:05:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/ReaderFTP.h"
#include "Modem/SIM868.h"
#include "Modem/Parser.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>


using namespace Parser;
using namespace std;


namespace ReaderFTP
{
    bool received_command = false;          // Если true, то команда принята, принимаем байты данных
    int pointer_command = 0;
    char buffer_command[32];

    int need_bytes = 0;                     // Столько байт должно быть принято
    int bytes_from_FTP = 0;                 // Столько FTP согласен отдать
    int pointer_data = 0;
    char buffer_data[SIZE_DATA_BUFFER];

    bool requested_bytes_received = false;
    bool received_FTPGET_1_0 = false;
}


void ReaderFTP::Reset()
{
    pointer_command = 0;
    received_command = false;

    pointer_data = 0;
    need_bytes = 0;

    requested_bytes_received = false;
}


void ReaderFTP::ReceiveBytes(int num_bytes)
{
    Reset();
    need_bytes = num_bytes;
    SIM868::Transmit::Format("AT+FTPGET=2,%d", need_bytes);
}

void ReaderFTP::AppendByte(char symbol)
{
    if (received_command)
    {
        buffer_data[pointer_data++] = symbol;

        if (pointer_data == bytes_from_FTP)
        {
            received_command = false;
            pointer_command = 0;
            requested_bytes_received = true;
        }
    }
    else
    {
        if (symbol == 0x0d)
        {

        }
        else if (symbol == 0x0a)
        {
            if (pointer_command)
            {
                char buffer[32];

                buffer_command[pointer_command++] = 0;

                pchar first_word = GetWord(buffer_command, 1, buffer);

                if (strcmp(first_word, "+FTPGET") == 0)
                {
                    pchar second_word = GetWord(buffer_command, 2, buffer);

                    if (second_word[0] == '1')
                    {
                        pchar third_word = GetWord(buffer_command, 3, buffer);
                        if (third_word[0] == '1')
                        {
                            SIM868::Transmit::Format("AT+FTPGET=2,%d", need_bytes);
                            pointer_command = 0;
                        }
                        else
                        {
                            received_FTPGET_1_0 = true;
                        }
                    }
                    else if (second_word[0] == '2')
                    {
                        pchar num_readed_bytes = GetWord(buffer_command, 3, buffer);

                        bytes_from_FTP = atoi(num_readed_bytes);

                        received_command = bytes_from_FTP > 0;

                        if (!received_command)
                        {
                            pointer_command = 0;
                        }
                    }
                }
                else
                {
                    received_command = false;
                    pointer_command = 0;
                }
            }
        }
        else
        {
            buffer_command[pointer_command++] = symbol;
        }
    }
}
