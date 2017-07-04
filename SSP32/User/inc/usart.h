#ifndef __USART_H
#define __USART_H

#ifndef  _USART_GLOBAL
#define  USART_EXT  extern 
#else
#define  USART_EXT 
#endif
#include "stdio.h"	
#include "sys.h" 

USART_EXT void uart1_init(u32 bound);
USART_EXT void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
USART_EXT void Usart1_Send(u8 *buf,u16 len);

USART_EXT void uart2_init(u32 bound);
USART_EXT void Usart2_Send(u8 *buf,u16 len);

USART_EXT u8 *copystring(u8 *des,u8 *src);
USART_EXT u16 get_sspbuf(u16 src_len);

#endif


