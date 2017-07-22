
#define _SENSORS_GPIO_GLOBAL

#include "includes.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
#include "stm32f10x.h"

#include "sensors.h"




// port A, bit 1           
#define PYD1798_OUT_LOW()  	(GPIOA->BRR = 0x00000002) 
#define PYD1798_OUT_HIGH() 	(GPIOA->BSRR = 0x00000002) 
#define PYD1798_IN_READ()  (GPIOA->IDR  & 0x0002)    


#define smokeModule_IN_READ()  (GPIOA->IDR  & 0x0020) 


/*-------------------------------------------------------------------------------------------


STM32的每个IO口都可以作为中断输入，要把IO口作为外部中断输入，有一下几个步骤：
第一步 初始化IO口作为输入
	设置你要作为外部中断输入的IO口的状态，可以设置为上拉/下拉输入，也可以设置为浮空输入，
但是浮空的时候一定要带上拉或者下拉电阻，否则可能导致中断不停的触发。在干扰较大的地方，
就算使用了上拉/下拉，也建议使用外部上拉/下拉电阻，这样可以一定程度防止外部干扰带来的影响。

第二步 开启IO口复用时钟，设置IO口于中断线的映射关系
	STM32的IO口与中断线的对应关系需要配置外部中断寄存器（EXTICR）,怎样我们要先开启复用时钟，
然后配置IO口与中断线的对应关系，才能把外部中断与中断线连接起来。

第三步 开启与该IO口相对的线上中断/事件，设置触发条件
	我们要配置中断的产生的条件，STM32可以配置成上升沿触发，下降沿触发，或者任意电平触发，
但是不能配置成高电平或者低电平触发。这里根据自己的实际情况来配置。
注：第二步与第三步可以作为一个整体来设置

第四步 配置中断分组（NVIC）,并且使能中断
	这一步，就是配置中断分组，以及使能。对STM32的中断来说，只有配置了NVIC的设置，并开启才能被
执行，否则是不会执行到中断服务函数里面去的。

第五步 编写中断服务函数
	这是中断设置的最后一步，中断服务函数，是必不可少的，如果代码里开启了中断，但是没编写中断服务
函数，就可能引起硬件错误，从而导致程序崩溃！所以在开启了某个中断后，一定要记得为改中断编写服务
函数。在中断服务函数中编写你要执行的中断后的操作。

----------------------------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------
	1、初始化引脚为上拉输入
------------------------------------------------------------------------*/
void PM25_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;         		/* PA4 */                                   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//输出频率最大50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			//带上拉电阻输出

	GPIO_Init(GPIOA, &GPIO_InitStructure);
      
}

/*-----------------------------------------------------------------------
	2、使能AFIO复用时钟功能
------------------------------------------------------------------------*/
void PM25_AFIO_config(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//使能时钟

}

/*-----------------------------------------------------------------------
	3、将GPIO口与中断线映射起来 
	开启与该IO口相对的线上中断/事件，设置触发条件
------------------------------------------------------------------------*/
void PM25_EXTILine_config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	 
	EXTI_ClearITPendingBit(EXTI_Line4);		//清空中断标志

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);		//选择中断管脚PA4
	 
	EXTI_InitStructure.EXTI_Line = EXTI_Line4; 						//选择中断线路4
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 			//设置为中断请求，非事件请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 	//设置中断触发方式为上下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//外部中断使能
	
	EXTI_Init(&EXTI_InitStructure);

}

/*-----------------------------------------------------------------------
	4、配置中断分组（NVIC）,并且使能中断
------------------------------------------------------------------------*/
void PM25_NVIC_config(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;	//定义结构体

//	NVIC_Configuration();	//前面已经设置了中断分组2

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; 		//使能外部中断所在的通道

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级 2， 

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 	//子优先级 2

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 	//使能外部中断通道 

	NVIC_Init(&NVIC_InitStructure); 		//根据结构体信息进行优先级初始化 


}







/*-----------------------------------------------------------------------
	5、中断服务函数
------------------------------------------------------------------------*/
void EXTI4_IRQHandler(void)
{
	
	sensors_printf("\n EXTI4_IRQHandler");
	   
	EXTI_ClearITPendingBit(EXTI_Line4);	//清除 LINE 上的中断标志位
	
}


/*-----------------------------------------------------------------------
	初始化
	使用外部中断方法，但由于pm2.5的输出周期为10ms，周期时间太短
	使用外部中断触发方法计数难以实现高精度
	故此方法暂不使用
------------------------------------------------------------------------*/
void PM25_EXTI_init(void)
{
	PM25_gpio_config();
	PM25_AFIO_config();
	PM25_EXTILine_config();
	PM25_NVIC_config();
}



/*-----------------------------------------------------------------------
	PYD1798
------------------------------------------------------------------------*/
void PYD1798_gpio_init(void)
{

	RCC->APB2ENR |= 0x00000004;  // I/O port A clock enabled

	GPIOA->CRL &= 0xFFFFFF0F;  // set output 
	GPIOA->CRL |= 0x00000010;  // 

}






/*-----------------------------------------------------------------------
	PYD1798
------------------------------------------------------------------------*/
void PYD1798_pin_out(void)
{

	GPIOA->CRL &= 0xFFFFFF0F;  
	GPIOA->CRL |= 0x00000010;  // 

}

/*-----------------------------------------------------------------------
	PYD1798
------------------------------------------------------------------------*/
void PYD1798_pin_in(void)
{

	GPIOA->CRL &= 0xFFFFFF0F;  
	GPIOA->CRL |= 0x00000080;  // 

}


/*-----------------------------------------------------------------------
	判断是否有人体移动
	连续10次数据递增或递减表示有人体移动，保持2s
	即连续2s内没有检测到一次连续10次递增或递减则清楚人体移动标志
------------------------------------------------------------------------*/
void remove_check(int chl)
{
	static int old_chl = 0;		/* 前一次的chl保存  */	
	static int dir = 0;			/* 前一比较是递增 1，还是递减 0  */
	static int count = 0;		/* 连续递增或递减次数  */
	static int uncount = 0;		/* 连续多少次没有检测到人体移动  */


	if(dir){					/* 前一次递增 */
		if(chl > old_chl){		/* 递增 */
			count++;
		}else{
			if(chl < old_chl){	/* 递减 */
				count = 1;
				dir = 0;
			}else{
				count = 0;	
			}
		}

	}else{

		if(chl < old_chl){		/* 递减 */
			count++;
		}else{
			if(chl > old_chl){	/* 递增 */
				count = 1;
				dir = 1;
			}else{
				count = 0;	
			}
		}

	}

	old_chl = chl;				/* 保存数据 */

	
	if(count >= 10){					/* 连续10次以上递增或递减 */
		sensors_value.removeFlag = 1;	/* 检测到人体移动 */
		uncount = 0;	
	}else{
		uncount++;
	}

	if((sensors_value.removeFlag == 1) && uncount > 200){	/* 连续200次没有检测到人体移动 */
		sensors_value.removeFlag = 0;	/* 取消人体移动标志 */

	}


}





/*-----------------------------------------------------------------------
	PYD1798
------------------------------------------------------------------------*/
void PYD1798_readdigipyro(void)
{
	int i, k, PIRval[3] = {0} ;	
	uint bitmask;
	
	PYD1798_pin_out(); 
	PYD1798_OUT_HIGH();

	delay_1us(150);
	for(k=0; k<2; k++) // get 14bits of DL data for each channel
	{
		bitmask = 0x2000; // Set BitPos
		PIRval[k] = 0;
		for (i=0; i < 14; i++)
		{
			// create low to high transition			
			PYD1798_OUT_LOW(); 	// Set DL = Low, Low level duration must be > 200 ns (tL)

			delay_1us(1);		// number of nop to be adapted to processor speed
			PYD1798_OUT_HIGH(); // Set DL = High, High level duration must be > 200 ns (tH)
			delay_1us(1); 

			PYD1798_pin_in(); 	// Configure DL as Input
			delay_1us(8); 		// Wait for stable signal
			if (PYD1798_IN_READ()) PIRval[k] |= bitmask; // If DL High set masked bit in PIRVal
			bitmask >>= 1;
			
			PYD1798_OUT_LOW();
			PYD1798_pin_out(); 
		}
		
	}
	
//	sensors_printf(" [%d](%d)(%d)", PIRval[0], PIRval[1], (PIRval[1] -6700)/80 + 25);
//	sensors_printf(" %d", PIRval[0]);

	PYD1798_OUT_LOW();  // Set DL = Low
	delay_1us(1);	

	PYD1798_pin_in(); 	 // Configure DL as Input
	
	remove_check(PIRval[0]);	/* 检测人体移动 */

}

/*-----------------------------------------------------------------------
	PYD1798
	每2ms执行一次,PYD1798每10ms读取一次
------------------------------------------------------------------------*/
void PYD1798_check(void)
{
	static int i = 0;

	i++;
	if(i >= 5){
		PYD1798_readdigipyro();
		i = 0;
	}

}


/*-----------------------------------------------------------------------
	烟雾引脚初始化
------------------------------------------------------------------------*/
void smokeModule_gpio_init(void)
{

	RCC->APB2ENR |= 0x00000004;  	// I/O port A clock enabled

	GPIOA->CRL &= 0xFF0FFFFF;  	// set PA5 INPUT
	GPIOA->CRL |= 0x00800000;  	// 

}

/*-----------------------------------------------------------------------
	烟雾检测
	每2ms执行一次
------------------------------------------------------------------------*/
void smokeModule_check(void)
{	
	if(smokeModule_IN_READ()){		
		sensors_value.smokeModule_value = 1;		
	}else{	
		sensors_value.smokeModule_value = 0;		
	}

}







