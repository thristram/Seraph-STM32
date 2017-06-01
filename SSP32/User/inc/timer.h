#ifndef __TIMER_H
#define __TIMER_H

#ifndef  _TIMER_GLOBAL
#define  TIMER_EXT  extern 
#else
#define  TIMER_EXT 
#endif
#include "sys.h"
#include "type.h"

TIMER_EXT union  FLAG 	SYSFlag1_;
#define SYS1Flag 					SYSFlag1_._flag_byte;
#define f_2ms							SYSFlag1_._flag_bit.bit0
#define f_100ms						SYSFlag1_._flag_bit.bit1
#define f_300ms						SYSFlag1_._flag_bit.bit2
#define f_1s							SYSFlag1_._flag_bit.bit3
#define f_2s							SYSFlag1_._flag_bit.bit4

TIMER_EXT u16 systime[4];


TIMER_EXT void TIM2_Init(void);
#endif




