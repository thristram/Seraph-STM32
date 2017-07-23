#define _SENSORS_TIMER_GLOBAL

#include "includes.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#include "sensors.h"


GP2Y1023_value_t GP2Y1023_value;





/*-----------------------------------------------------------------------
	1、初始化引脚为上拉输入
------------------------------------------------------------------------*/
void GP2Y1023_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;         		/* PA7 */                                   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//输出频率最大50MHz
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			//带上拉电阻输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	

	GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
}


/*-----------------------------------------------------------------------
	2、配置中断分组（NVIC）,并且使能中断
------------------------------------------------------------------------*/
void GP2Y1023_NVIC_config(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;	//定义结构体

//	NVIC_Configuration();	//前面已经设置了中断分组2
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 		 //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //抢占优先级 1， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 	//子优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 	//使能外部中断通道 

	NVIC_Init(&NVIC_InitStructure); 		//根据结构体信息进行优先级初始化 

}




/*-----------------------------------------------------------------------
	3、初始化引脚为上拉输入
------------------------------------------------------------------------*/
void GP2Y1023_timer_config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_DeInit(TIM3); //初始化TIM3为缺省值 0

	TIM_TimeBaseStructure.TIM_Period = 0xffff; 
	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);



 	//下面是对 TIM_ICInitStructure的配置
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;    
	TIM_ICInitStructure.TIM_ICFilter = 0x0;      

	TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);  
		
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI2FP2);  		/* Select the TIM2 Input Trigger: TI2FP2 */  
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);  	/* Select the slave Mode: Reset Mode */  
	TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);  
	
	/* TIM enable counter */  
	TIM_Cmd(TIM3, ENABLE);	
	
	/* Enable the CC2 Interrupt Request */	
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE); 


}





/*-----------------------------------------------------------------------
	粉尘浓度平滑
	每存储10个数据之后才计算浓度值
	前面10个浓度的值的和保存在out_us_old中；
------------------------------------------------------------------------*/
void pm25_density_handler(int output_us)
{
	static int out_us[16] = {0};
	static int out_us_old = 0;
	static int out_us_cnt = 0;
	int i;


	if(out_us_cnt >= 10){

		for(i=1; i<10; i++){				/* 10 个数据的和 */
			out_us[0] += out_us[i];
		}

		if(out_us_old){
			
			out_us[0] = (out_us_old + out_us[0]) / 2 ;		/* 前后20个数据的*/

		}
		
		sensors_value.pm25_density = (out_us[0] / 10 + 1 - OUTPUT_AT_NO_DUST) / OUTPUT_SENSITIVITY_K;
		sensors_printf("\n pm25:%lf", sensors_value.pm25_density);

		out_us_old = out_us[0];
		
		out_us[0] = output_us;
		out_us_cnt = 1;		
		
	}else{

		out_us[out_us_cnt] = output_us;
		out_us_cnt++;

	}


}



/*-----------------------------------------------------------------------
	4、中断服务函数
------------------------------------------------------------------------*/
void TIM3_IRQHandler(void)
{

	TIM_ClearITPendingBit(TIM3, TIM_IT_CC2); 	//清除TIM3的中断待处理位
	
	GP2Y1023_value.IC2Value = TIM_GetCapture2(TIM3); //读取IC2捕获寄存器的值，即为PWM周期的计数值
	GP2Y1023_value.IC1Value = TIM_GetCapture1(TIM3); //读取IC2捕获寄存器的值，即为PWM周期的计数值

	if(GP2Y1023_value.IC2Value != 0){
		

		GP2Y1023_value.dutyCycle = (float)GP2Y1023_value.IC1Value / GP2Y1023_value.IC2Value; //读取IC1捕获寄存器的值，并计算占空比
		GP2Y1023_value.frequency = (float)GP2Y1023_value.timerFreq / GP2Y1023_value.IC2Value; //计算PWM频率。
		GP2Y1023_value.output_us = GP2Y1023_value.dutyCycle*1000000/GP2Y1023_value.frequency;


//		sensors_printf("\n GP2Y1023 IC2Value:%d	IC1Value:%d	dutyCycle:%lf	frequency:%lf output_us:%d", 
//			GP2Y1023_value.IC2Value, TIM_GetCapture1(TIM3), GP2Y1023_value.dutyCycle, GP2Y1023_value.frequency, GP2Y1023_value.output_us);

		/* 取值范围 */
		if(GP2Y1023_value.output_us < OUTPUT_AT_NO_DUST){
			GP2Y1023_value.output_us = OUTPUT_AT_NO_DUST;
		}	
		if(GP2Y1023_value.output_us > OUTPUT_AT_MAX_DUST){
			GP2Y1023_value.output_us = OUTPUT_AT_MAX_DUST;
		}
		pm25_density_handler(GP2Y1023_value.output_us);

	}else{
	
		GP2Y1023_value.dutyCycle = 0;
		GP2Y1023_value.frequency = 0;

	}
	
	
}


/*-----------------------------------------------------------------------
	初始化
------------------------------------------------------------------------*/
void GP2Y1023_pwm_init(void)
{
	RCC_ClocksTypeDef	RCC_Clocks;

	GP2Y1023_gpio_config();

	GP2Y1023_NVIC_config();

	GP2Y1023_timer_config();

	RCC_GetClocksFreq(&RCC_Clocks);  
	
	GP2Y1023_value.timerFreq = RCC_Clocks.SYSCLK_Frequency / 720;
//	sensors_printf("\n RCC_Clocks HCLK:%dHz SYSCLK:%dHz GP2Y1023_value.timerFreq:%dHz", RCC_Clocks.HCLK_Frequency, 
//			RCC_Clocks.SYSCLK_Frequency, GP2Y1023_value.timerFreq);
//	
}






