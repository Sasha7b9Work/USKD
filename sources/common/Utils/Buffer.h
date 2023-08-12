// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Mutex.h"
#include <cstring>


template<int capacity>
class Buffer
{
public:

    Buffer() : size(0) {}; //-V730

    char *Data() { return buffer; }

    const char *DataConst() const { return buffer; }

    char *Last()
    {
        return buffer + Size();
    }

    // Возвращает количество элементов в буфере
    int Size() const
    {
        return size;
    }

    void Clear()
    {
        size = 0;
    }

    bool Append(const void *data, int _size)
    {
        if (Size() + _size > capacity)
        {
            LOG_ERROR("!!!buffer is full!!!");
            return false;
        }

        std::memcpy(&buffer[size], data, (uint)_size);
        size += _size;

        return true;
    }

    bool Append(char symbol)
    {
        if (size < capacity)
        {
            buffer[size++] = symbol;
            return true;
        }

        LOG_ERROR("!!!buffer is full!!!");
        return false;
    }

    // Удалить первых n элементов
    void RemoveFirst(int n)
    {
        std::memmove(buffer, buffer + n, (uint)(size - n));
        size -= n;
    }

    char &operator[](uint i)
    {
        if ((int)i >= 0 && (int)i < Size())
        {
            return buffer[i];
        }

        static char null(0);

        return null;
    }

    char &operator[](int i)
    {
        if (i >= 0 && i < Size())
        {
            return buffer[i];
        }

        static char null(0);

        return null;
    }

    Mutex mutex;

protected:

    int size;

    char buffer[capacity];
};
