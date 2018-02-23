
#define _USART_GLOBAL
#include "includes.h"
#include "stdio.h"
#include "stdlib.h"




USART2_handle_t USART2_handle = {0};



#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
	int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
_sys_exit(int x)
{
	x = x;
}
//重定义fputc函数

int fputc(int ch, FILE *f)
{
	while((USART2->SR & 0X40)==0);//循环发送,直到发送完毕
	USART2->DR = (u8)ch;
	return ch;
}

#endif


/*-------------------------------------------------------------------------
//bound:波特率    
-------------------------------------------------------------------------*/
void USART1_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

	USART_DeInit(USART1);  //复位串口1
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启空闲中断
	USART_ClearFlag(USART1,USART_IT_TC);					//清除发送完成标志
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);   //使能串口1 DMA接收
	USART_Cmd(USART1, ENABLE);                    //使能串口

	//相应的DMA配置
	DMA_DeInit(DMA1_Channel5);   //将DMA的通道5寄存器重设为缺省值  串口1对应的是DMA通道5
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)DMA_Rece_Buf;  	//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  	//数据传输方向，从外设读取发送到内存
	DMA_InitStructure.DMA_BufferSize = DMA_BUF_LEN;  		//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  		//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  		//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);  		//根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器

	DMA_Cmd(DMA1_Channel5, ENABLE);  //正式驱动DMA传输

}

/*-------------------------------------------------------------------------    
	//重新恢复DMA指针
-------------------------------------------------------------------------*/
void MYDMA_Enable(DMA_Channel_TypeDef * DMA_CHx)
{
	DMA_Cmd(DMA_CHx, DISABLE );  	//关闭USART1 TX DMA1 所指示的通道
	DMA_SetCurrDataCounter(DMA_CHx, DMA_BUF_LEN);		//DMA通道的DMA缓存的大小
	DMA_Cmd(DMA_CHx, ENABLE);  	//使能USART1 TX DMA1 所指示的通道
}

//字符串复制
u8 * copystring(u8 *des,u8 *src)
{
	u8 *tmp=des;
	while( *tmp++ = *src++ ) ;
	return des;
}


//发送len个字节.
//buf:发送区首地址
//len:发送的字节数
/*
void Usart1_Send(u8 *buf,u8 len)
{
	u8 t;
  	for(t=0;t<len;t++)		//循环发送数据
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(USART1,buf[t]);
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}
*/
void Usart1_Send(u8 *buf,u16 len)
{
	if(len >= USART1_TXBUF_LEN)	len = USART1_TXBUF_LEN;
	mymemcpy(Usart1_Send_Buf, buf,len);
	Usart1_Send_Length = len;
	Usart1_Send_Cnt = 0;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);    //这里立即开启发送中断
}




/*-------------------------------------------------------------------------
//从接收缓存中取出以BB BB开头，以0A 0A结尾的数据包
-------------------------------------------------------------------------*/
u16 get_sspbuf(u16 src_len)
{
	u16 i, ret = 0;

	if((DMA_Rece_Buf[0] == 0xAA) && (DMA_Rece_Buf[1] == 0xAA))
	{

		ss.flag.bit.tcp_en = (DMA_Rece_Buf[6] == 0x01) ? 1 : 0;
	}

	if((DMA_Rece_Buf[0] == 0xBB) && (DMA_Rece_Buf[1] == 0xBB))
	{

		if((DMA_Rece_Buf[src_len -2] == 0x0A) && (DMA_Rece_Buf[src_len -1] == 0x0A))
		{

			ret = src_len - 2;
			for(i = 0; i < ret; i++)
			{
				ssp_buf[i] = DMA_Rece_Buf[i];
			}
			ssp_buf[ret] = 0;
		}
	}

	return ret;

}


/*-------------------------------------------------------------------------
//串口中断函数
-------------------------------------------------------------------------*/
void USART1_IRQHandler(void)                	//串口1中断服务程序
{

	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET){

		USART_ReceiveData(USART1);					//读取数据 注意：这句必须要，否则不能够清除中断标志位。我也不知道为啥！

		Usart1_Rec_Cnt = DMA_BUF_LEN -DMA_GetCurrDataCounter(DMA1_Channel5);		//算出接本帧数据长度

		ssp_length = get_sspbuf(Usart1_Rec_Cnt);
		rev_success = 1;
		Usart1_Delay_Cnt = 0;

		USART_ClearITPendingBit(USART1, USART_IT_IDLE);         //清除中断标志
		MYDMA_Enable(DMA1_Channel5);                   			//恢复DMA指针，等待下一次的接收
	}

	/* //发送中断(发送寄存器为空时) */
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET){

		if(Usart1_Send_Cnt < Usart1_Send_Length)
		{

			USART1->DR = Usart1_Send_Buf[Usart1_Send_Cnt]&0x01FF;
			Usart1_Send_Cnt++;
		}
		else
		{

			Usart1_Send_Cnt = 0;
			if(!ssp_uart_mutex)	deleteNodeFromUartTxSLHead();//互斥量，uartRxSLHead和uartRxSLLast在addNodeToUartRxSLLast函数中未处理完就被清
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	//关闭发送中断
		}
	}

}



/*-------------------------------------------------------------------------
    //bound:波特率
-------------------------------------------------------------------------*/
void USART2_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//使能USART2时钟

	USART_DeInit(USART2);  //复位串口1

	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9

	//USART2_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;		//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ClearFlag(USART2,USART_IT_TC);					//清除发送完成标志
	USART_Cmd(USART2, ENABLE);                    //使能串口

}


/*-------------------------------------------------------------------------
//buf:发送区首地址
//len:发送的字节数
-------------------------------------------------------------------------*/
void USART2_send(u8 *buf, u16 len)
{
	if(len >= USART2_TX_BUF_LEN) len = USART2_TX_BUF_LEN;
	
	mymemcpy(USART2_handle.txBuf, buf, len);
	
	USART2_handle.txLen = len;
	USART2_handle.txCnt = 0;
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);    //这里立即开启发送中断
}


/*----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/
void USART2_recv(void)
{
	u8 temp;
	u8 check_sum;
	
	temp = (u8)USART_ReceiveData(USART2);
	
	USART2_handle.rxBuf[USART2_handle.rxCnt] = temp;
	USART2_handle.rxCnt++;
	switch(USART2_handle.rxCnt)
	{
		case 1:
			if ((temp != 0xEE) && (temp != 0xDD))	USART2_handle.rxCnt = 0;
			break;
		case 2:
			if ((temp != 0xEE) && (temp != 0xDD) && (temp != 0xAA)) USART2_handle.rxCnt = 0;
			break;
		case 3:
			//if (!temp)				rev_count = 0;
			break;
		case 4:
			//if (!temp)				rev_count = 0;
			break;
		case 5:
			//if (!temp)				rev_count = 0;
			break;
		default:
			if (USART2_handle.rxCnt >= USART2_RX_BUF_LEN){		//防止接收错误后溢出
				USART2_handle.rxCnt = 0;
			}			
			if ((USART2_handle.rxBuf[0] == 0xEE) && ((USART2_handle.rxBuf[1] == 0xEE) || (USART2_handle.rxBuf[1] == 0xAA))){

				if (USART2_handle.rxCnt > USART2_handle.rxBuf[5] + 2){		//接收数据完成
				
					check_sum = Check_Sum(USART2_handle.rxBuf + 2, USART2_handle.rxBuf[5]);					
					if (check_sum == USART2_handle.rxBuf[USART2_handle.rxBuf[5] + 2]){		//校验正确
						SICP_handle.status.bit.recvFlag = 1;
						memcpy(SICP_handle.rxBuf, USART2_handle.rxBuf, USART2_handle.rxCnt);
					}
					USART2_handle.rxCnt = 0;

				}
			}
			else if((USART2_handle.rxBuf[0] == 0xDD) && (USART2_handle.rxBuf[1] == 0xDD)){
				
				if (USART2_handle.rxCnt > USART2_handle.rxBuf[3] + 2){		//接收数据完成		
					SICP_handle.status.bit.recvFlag = 1;
					memcpy(SICP_handle.rxBuf, USART2_handle.rxBuf, USART2_handle.rxCnt);
					USART2_handle.rxCnt = 0;
				}
			}
			else{
				
				USART2_handle.rxCnt = 0;
			}
			break;
			
	}
	
}


/*-------------------------------------------------------------------------
//串口中断函数
-------------------------------------------------------------------------*/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){

		USART2_recv();
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}

	/* //发送中断(发送寄存器为空时) */
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET){

		if(USART2_handle.txCnt < USART2_handle.txLen){
			USART2->DR = USART2_handle.txBuf[USART2_handle.txCnt] & 0x01FF;
			USART2_handle.txCnt++;

		}else{
			USART2_handle.txCnt = 0;
			if(!sicp_uart_mutex)	deleteNodeFromUart2TxSLHead();		//互斥量，uartRxSLHead和uartRxSLLast在addNodeToUartRxSLLast函数中未处理完就被清
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);			//关闭发送中断

		}
	}

}

























