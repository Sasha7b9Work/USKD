#define VERSION 111


// Сообщения лога записываются в буфер
//#define SOFTWARE_LOG

#define ENABLE_LOG

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint;
typedef const char *pchar;
typedef unsigned char uchar;


#define _bitset(bits)                               \
  ((uint8)(                                         \
  (((uint8)((uint)bits / 01)        % 010) << 0) |  \
  (((uint8)((uint)bits / 010)       % 010) << 1) |  \
  (((uint8)((uint)bits / 0100)      % 010) << 2) |  \
  (((uint8)((uint)bits / 01000)     % 010) << 3) |  \
  (((uint8)((uint)bits / 010000)    % 010) << 4) |  \
  (((uint8)((uint)bits / 0100000)   % 010) << 5) |  \
  (((uint8)((uint)bits / 01000000)  % 010) << 6) |  \
  (((uint8)((uint)bits / 010000000) % 010) << 7)))

#define BINARY_U8( bits ) _bitset(0##bits)


#define _GET_BIT(value, bit)   (((value) >> (bit)) & 0x01)
#define _SET_BIT(value, bit)   ((value) |= (1 << (bit)))
#define _CLEAR_BIT(value, bit) ((value) &= (~(1 << (bit))))


union BitSet32
{
    BitSet32(uint _word) : word(_word) { }
    uint   word;
    uint8  byte[4];
};


#ifndef WIN32
#define nullptr 0 //-V1059
#pragma diag_suppress 2748
#pragma anon_unions
#else
#define __ISB()
#define __DSB()
#define __disable_irq()
#define __enable_irq()
#define __asm
#ifndef GUI
#define asm(x)
#endif
#endif


#include "Utils/Log.h"
