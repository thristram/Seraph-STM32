#ifndef __DELAY_H
#define __DELAY_H

#ifndef  _DELAY_GLOBAL
#define  DELAY_EXT  extern 
#else
#define  DELAY_EXT 
#endif
#include "sys.h"

DELAY_EXT void delay_init(void);
DELAY_EXT void delay_ms(u16 nms);
DELAY_EXT void delay_us(u32 nus);

#endif





























