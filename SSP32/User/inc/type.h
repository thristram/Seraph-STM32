//*************************************************************
#ifndef  TYPE_H_
#define  TYPE_H_
//*************************************************************

#ifndef  _TYPE_GLOBAL
#define  TYPE_EXT  extern 
#else
#define  TYPE_EXT 
#endif
#include "sys.h"
union FLAG   
{
    struct
    {
        u8 bit0:1;
        u8 bit1:1;
        u8 bit2:1;
        u8 bit3:1;
        u8 bit4:1;
        u8 bit5:1;
        u8 bit6:1;
        u8 bit7:1;
    }_flag_bit;
    u8 _flag_byte;
};


#endif


