#ifndef __LED_H
#define __LED_H

#ifndef  _LED_GLOBAL
#define  LED_EXT  extern 
#else
#define  LED_EXT 
#endif

#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//#define SYSTEM_LED_ON			(GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_SET))
//#define SYSTEM_LED_OFF			(GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_RESET))
//#define SYSTEM_LED_REVERSE		(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_9))?(GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_RESET)):(GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_SET))

#define PA4_ON			(GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET))
#define PA4_OFF			(GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET))
#define PA4_TEST			(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_4))?(GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET)):(GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET))

/* test */
#define SYSTEM_LED_ON			PA4_ON
#define SYSTEM_LED_OFF			PA4_OFF
#define SYSTEM_LED_REVERSE		PA4_TEST


extern u8	cmd_led_flag;


#define LED1_RCC_CLOCK			RCC_APB2Periph_GPIOC
#define LED1_GPIO_BASE			GPIOC
#define LED1_GPIO_PIN			GPIO_Pin_4
#define LED1_ON			(LED1_GPIO_BASE->BSRR = LED1_GPIO_PIN)
#define LED1_OFF			(LED1_GPIO_BASE->BRR = LED1_GPIO_PIN)
#define LED1_REVERSE		(GPIO_ReadOutputDataBit(LED1_GPIO_BASE,LED1_GPIO_PIN))?(GPIO_WriteBit(LED1_GPIO_BASE,LED1_GPIO_PIN,Bit_RESET)):(GPIO_WriteBit(LED1_GPIO_BASE,LED1_GPIO_PIN,Bit_SET))

#define LED2_RCC_CLOCK			RCC_APB2Periph_GPIOC
#define LED2_GPIO_BASE			GPIOC
#define LED2_GPIO_PIN			GPIO_Pin_5
#define LED2_ON			(LED2_GPIO_BASE->BSRR = LED2_GPIO_PIN)
#define LED2_OFF			(LED2_GPIO_BASE->BRR = LED2_GPIO_PIN)
#define LED2_REVERSE		(GPIO_ReadOutputDataBit(LED2_GPIO_BASE,LED2_GPIO_PIN))?(GPIO_WriteBit(LED2_GPIO_BASE,LED2_GPIO_PIN,Bit_RESET)):(GPIO_WriteBit(LED2_GPIO_BASE,LED2_GPIO_PIN,Bit_SET))






#define LED1_GREEN		(0XFF000000)
#define LED1_RED			(0X00FF0000)
#define LED1_BLUE			(0X0000FF00)
#define LED1_YELLOW		(LED1_GREEN | LED1_RED)
#define LED1_MAGENTA		(LED1_BLUE | LED1_RED)		/* Ʒ�� */
#define LED1_CYAN		(LED1_GREEN | LED1_BLUE)		/* ��ɫ */
#define LED1_WHITE		(LED1_GREEN | LED1_BLUE | LED1_RED)		



typedef struct{

	u8	green;
	u8	reg;
	u8	blue;

} LED_show_t;

















void system_led_Init(void);//��ʼ��
void system_led_show(void);


void LED_Init(void);


void LED_test(void);


		 				    
#endif
