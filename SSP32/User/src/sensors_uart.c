
#define _SENSORS_TIMER_GLOBAL

#include "includes.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#include "sensors.h"

CO2_uart_t CO2_uart;




/*-----------------------------------------------------------------------
	1、配置时钟
------------------------------------------------------------------------*/
void CO2_RCC_config(void)
{

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

}


/*-----------------------------------------------------------------------
	2、配置中断
------------------------------------------------------------------------*/
void CO2_NVIC_config(void) 
{ 
	NVIC_InitTypeDef NVIC_InitStructure;
 
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4 ;//抢占优先级3
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;		//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
  	NVIC_Init(&NVIC_InitStructure);		//根据指定的参数初始化VIC寄存器

}


/*-----------------------------------------------------------------------
	3、初始化引脚为
------------------------------------------------------------------------*/
void CO2_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure); 		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
  
}


/*-----------------------------------------------------------------------
	4、初始化uart3
------------------------------------------------------------------------*/
void CO2_usart3_config(void)
{
  	USART_InitTypeDef USART_InitStructure;			//定义一个包含串口参数的结构体
  
  	USART_DeInit(USART3);  

	USART_InitStructure.USART_BaudRate = 9600; 		//波特率9600
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8位数据位
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//1位停止位
  	USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//输入加输出模式
  	
    USART_Init(USART3, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);    

}


/*-----------------------------------------------------------------------
	CO2初始化
------------------------------------------------------------------------*/
void CO2_usart3_init(void)
{
	mymemset(&CO2_uart, 0, sizeof(CO2_uart_t));
	
	CO2_RCC_config();
	CO2_NVIC_config();
	CO2_gpio_config();
	CO2_usart3_config();

}


/*-----------------------------------------------------------------------
	生产命令的校验
------------------------------------------------------------------------*/
uint8_t CO2_usart3_checkSum(uint8_t *packet)
{
	uint8_t i, checksum = 0;
	for( i = 1; i < 8; i++)
	{
		checksum += packet[i];
	}
	checksum = 0xff - checksum;
	checksum += 1;
	
	return checksum;
}


/*-----------------------------------------------------------------------
	生产命令
------------------------------------------------------------------------*/
void CO2_usart3_cmd(uint8_t which)
{
	mymemset(CO2_uart.txBuf, 0, CO2_TXRX_DATA_LEN);
	CO2_uart.txLen = CO2_TXRX_DATA_LEN;
	CO2_uart.rxLen = 0;
	CO2_uart.hasTxLen = 0;

	CO2_uart.txBuf[0] = 0xff;
	CO2_uart.txBuf[1] = 0x01;

	switch(which){

		case 1:		
			CO2_uart.txBuf[2] = 0x86;			/* CO2_uart.readFlag */
			break;
				
		case 2:							
			CO2_uart.txBuf[2] = 0x87;	 		/* CO2_uart.zeroFlag */
			break;

		case 3:		
			CO2_uart.txBuf[2] = 0x88;			/* CO2_uart.spanFlag */
			CO2_uart.txBuf[3] = CO2_uart.spanHigh;			
			CO2_uart.txBuf[4] = CO2_uart.spanLow;			
			break;

		case 4:		
			CO2_uart.txBuf[2] = 0x79;	 		/* CO2_uart.atuoFlag */
			CO2_uart.txBuf[3] = CO2_uart.atuoValue;	 		
			break;
			
		case 5:		
			CO2_uart.txBuf[2] = 0x99;			/* CO2_uart.setrFlag */
			CO2_uart.txBuf[3] = CO2_uart.setrHigh;	 		
			CO2_uart.txBuf[4] = CO2_uart.setrLow;	 		
			break;

		default:
			break;

	}
	
	CO2_uart.txBuf[8] = CO2_usart3_checkSum(CO2_uart.txBuf);			

}



/*-----------------------------------------------------------------------
	CO2发送命令
	每100ms执行一次，不可随意修改周期
------------------------------------------------------------------------*/
void CO2_usart3_send(void)
{
	uint8_t which = 0;

	CO2_uart.readCtrl++;
	if(CO2_uart.readCtrl >= 10){	/* 1s */
		CO2_uart.readCtrl = 0;
		CO2_uart.readFlag = 1;
	}
	
	if(CO2_uart.sendFlag == 0){		/*  没有数据发送时 */

		if(CO2_uart.setrFlag == 1){
			CO2_uart.setrFlag = 0;
			which = 5;
			
		}else if(CO2_uart.atuoFlag == 1){
			CO2_uart.atuoFlag = 0;
			which = 4;
		
		}else if(CO2_uart.spanFlag == 1){
			CO2_uart.spanFlag = 0;
			which = 3;
	
		}else if(CO2_uart.zeroFlag == 1){
			CO2_uart.zeroFlag = 0;
			which = 2;
		
		}else if(CO2_uart.readFlag == 1){
			CO2_uart.readFlag = 0;
			which = 1;
		
		}else{
		
		}
			
		/* 发送第一个字节 */
		if(which > 0){
			
			CO2_uart.sendFlag = 1;
			CO2_usart3_cmd(which);			
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE);	//开启发送中断
						
		}

	}

}


/*-----------------------------------------------------------------------
	等待一定的延迟20ms
	每2ms执行一次
	暂不适用
------------------------------------------------------------------------*/
void CO2_usart3_send_finish_confirm(void)
{
	static uint8_t count = 0;

	if(CO2_uart.sendFlag == 2){
		
		if(count++ >= 10){
			CO2_uart.sendFlag = 0;
			count = 0;
		}

	}else{
		count = 0;
	}

}

/*-----------------------------------------------------------------------
	零点校准命令
------------------------------------------------------------------------*/
void CO2_zero_config(void)
{
	CO2_uart.zeroFlag = 1;
}

/*-----------------------------------------------------------------------
	校准SPAN 点命令
	spanValue为校准的值
------------------------------------------------------------------------*/
void CO2_SPAN_config(uint16_t spanValue)
{
	CO2_uart.spanFlag = 1;
	CO2_uart.spanHigh = (uint8_t)(spanValue >> 8) & 0xff;
	CO2_uart.spanLow = (uint8_t)spanValue & 0xff;
	
}

/*-----------------------------------------------------------------------
	开启/关闭自动校零
	atuoValue为1表示开启，为0表示关闭
------------------------------------------------------------------------*/
void CO2_atuo_config(uint16_t atuoValue)
{
	CO2_uart.atuoFlag= 1;
	CO2_uart.atuoValue = atuoValue ? 0xa0 : 0x00;
	
}

/*-----------------------------------------------------------------------
	设置量程
	setrValue为量程的值
------------------------------------------------------------------------*/
void CO2_setr_config(uint16_t setrValue)
{
	CO2_uart.setrFlag = 1;
	CO2_uart.setrHigh = (uint8_t)((setrValue >> 8) & 0xff);
	CO2_uart.setrLow = (uint8_t)(setrValue & 0xff);	
}




/*-----------------------------------------------------------------------
	uart3中断服务程序 
------------------------------------------------------------------------*/
void USART3_IRQHandler(void)                
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){		 //接收中断
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);        	 //清除中断标志
		CO2_uart.rxBuf[CO2_uart.rxLen] = (uint8_t)USART_ReceiveData(USART3);
		CO2_uart.rxLen++;
		if(CO2_uart.rxLen >= CO2_TXRX_DATA_LEN){		/* 接收完成 */

			if(CO2_uart.rxBuf[1] == 0x86){		/* 读取数据 */
				CO2_uart.readValue = CO2_uart.rxBuf[2] * 256 + CO2_uart.rxBuf[3];
				sensors_value.co2_density = (int)CO2_uart.readValue;
//				sensors_printf("\n CO2_uart.readValue:%d ppm", CO2_uart.readValue);
			}


//			sensors_printf("\n %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
//				CO2_uart.rxBuf[0], CO2_uart.rxBuf[1], CO2_uart.rxBuf[2], CO2_uart.rxBuf[3],
//				CO2_uart.rxBuf[4], CO2_uart.rxBuf[5], CO2_uart.rxBuf[6], CO2_uart.rxBuf[7],
//				CO2_uart.rxBuf[8]);

		}
		
	}

	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET){		//发送中断(发送寄存器为空时)

		if(CO2_uart.hasTxLen < CO2_uart.txLen){			/* 还没发送完成 */

				USART3->DR = CO2_uart.txBuf[CO2_uart.hasTxLen] & (uint16_t)0x01FF;
				CO2_uart.hasTxLen++;

		}else{
		
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);	//关闭发送中断
			CO2_uart.sendFlag = 0;	/* */

		}	
	}
	
} 













