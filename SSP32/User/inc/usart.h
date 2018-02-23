#ifndef __USART_H
#define __USART_H

#ifndef  _USART_GLOBAL
#define  USART_EXT  extern 
#else
#define  USART_EXT 
#endif
#include "stdio.h"	
#include "sys.h" 



#define USART1_DMA_CHANNEL       DMA1_Channel5






#define USART2_RX_BUF_LEN		64			/* usart2最大接收长度 */
#define USART2_TX_BUF_LEN		64			/* usart2最大发送长度 */

typedef struct
{
	u8		rxCnt;
	u8		txCnt;

	u8		rxLen;
	u8		txLen;
	
	u8		txBuf[USART2_TX_BUF_LEN];	
	u8		rxBuf[USART2_RX_BUF_LEN];

}USART2_handle_t;

extern USART2_handle_t USART2_handle;















void USART1_init(u32 bound);
USART_EXT void Usart1_Send(u8 *buf,u16 len);

USART_EXT u8 *copystring(u8 *des,u8 *src);




void USART2_init(u32 bound);
void USART2_send(u8 *buf, u16 len);















#endif


