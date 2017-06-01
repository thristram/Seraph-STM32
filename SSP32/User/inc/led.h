#ifndef __LED_H
#define __LED_H

#ifndef  _LED_GLOBAL
#define  LED_EXT  extern 
#else
#define  LED_EXT 
#endif

#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED1_Toggle		(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_9))?(GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_RESET)):(GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_SET))
#define LED2_Toggle		(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_10))?(GPIO_WriteBit(GPIOC,GPIO_Pin_10,Bit_RESET)):(GPIO_WriteBit(GPIOC,GPIO_Pin_10,Bit_SET))
#define LED3_Toggle		(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_11))?(GPIO_WriteBit(GPIOC,GPIO_Pin_11,Bit_RESET)):(GPIO_WriteBit(GPIOC,GPIO_Pin_11,Bit_SET))
#define LED4_Toggle		(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_12))?(GPIO_WriteBit(GPIOC,GPIO_Pin_12,Bit_RESET)):(GPIO_WriteBit(GPIOC,GPIO_Pin_12,Bit_SET))


LED_EXT void LED_Init(void);//初始化

		 				    
#endif
