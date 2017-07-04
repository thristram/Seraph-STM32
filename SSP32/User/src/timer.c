#define  _TIMER_GLOBAL
#include "includes.h"

void TIM2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 200;//¼ÆÊ±2ms
	TIM_TimeBaseStructure.TIM_Prescaler =720-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	
	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		f_2ms = 1;
		systime[0]++;
		if(systime[0] >= 50)
		{
			systime[0] = 0;
			f_100ms = 1;
		}
		systime[1]++;
		if(systime[1] >= 150)
		{
			systime[1] = 0;
			f_300ms = 1;
		}
		systime[2]++;
		if(systime[2] >= 500)
		{
			systime[2] = 0;
			f_1s = 1;
		}
		systime[3]++;
		if(systime[3] >= 1000)
		{
			systime[3] = 0;
			f_2s = 1;
		}
	}

}

