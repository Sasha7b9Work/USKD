// 2023/05/20 09:08:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>


using namespace std;


static unsigned int Hash(unsigned int, char);


int main(int argc, char *argv[])
{
    fstream input;

    const char *file_name = "../../../Device/Meter.bin";        // Запускаем из студии

    if (argc > 1)
    {
        file_name = argv[1];                                    // Запускаем из каталога *.bin с именем входного файла
    }

    input.open(file_name, ios_base::in | ios_base::binary);

    char byte = 0;

    unsigned int hash = 0;

    while (input.read(&byte, sizeof(byte)))
    {
        hash = Hash(hash, byte);
    }

    input.close();

    ifstream input_ver;             // *.h - файл с версией ПО

    if (argc < 2)
    {
        input_ver.open("../../../common/common_defines.h", ios_base::in);
    }
    else
    {
        input_ver.open("../common/common_defines.h", ios_base::in);
    }

    string line;

    input_ver >> line;
    input_ver >> line;
    input_ver >> line;

    input_ver.close();

    int version = atoi(line.c_str());

    ofstream out_file;
    out_file.open("Meter.frm", ios_base::out | ios_base::binary | ios_base::trunc);

    out_file.write((const char *)&version, 4);

    out_file.write((const char *)&hash, 4);

    input.open(file_name, ios_base::in | ios_base::binary);

    while (input.read(&byte, sizeof(byte)))
    {
        out_file.write(&byte, 1);
    }
}


unsigned int Hash(unsigned int hash, char byte)
{
    return (unsigned char)byte + (hash << 6) + (hash << 16) - hash;
}
