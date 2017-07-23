
#define _SENSORS_ADC_GLOBAL

#include "includes.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#include "sensors.h"

/*-----------------------------------------------------------------------
	ADC1_ConvertedValue[0]:ADC_Channel_10/ PC0
	ADC1_ConvertedValue[1]:ADC_Channel_11/ PC1
	ADC1_ConvertedValue[2]:ADC_Channel_12/ PC2
------------------------------------------------------------------------*/
__IO uint16_t ADC1_ConvertedValue[ADC1_SCAN_CHANNEL_NUM];



/*-----------------------------------------------------------------------
	使能DMA1/ADC1的时钟
	初始化模拟引脚
------------------------------------------------------------------------*/
void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);

	
	/* Configure PC1	as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);				

	/* Configure PA0 as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);				


	
}


/*-----------------------------------------------------------------------
	ADC1模式设置
------------------------------------------------------------------------*/
void ADC1_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	ADC_DeInit(ADC1); //将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 		//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode =ENABLE; 			//模数转换工作在扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 		//模数转换工作在连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 	//外部触发转换关闭
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = ADC1_SCAN_CHANNEL_NUM; 			//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure); 					//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

	// 配置ADC时钟，为PCLK2的8分频，即9MHz 
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	//ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0,  1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5 );
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5 );

	ADC_DMACmd(ADC1, ENABLE); 	/* Enable ADC1 DMA */ 
	
	ADC_Cmd(ADC1, ENABLE); 		/* Enable ADC1 */ 
	
	ADC_ResetCalibration(ADC1); /* 复位校准寄存器 */ 
	
	/* 等待校准寄存器复位完成 */ 
	while(ADC_GetResetCalibrationStatus(ADC1)); 
	
	ADC_StartCalibration(ADC1); 			/* ADC校准 */ 
	
	while(ADC_GetCalibrationStatus(ADC1)); 	/* 等待校准完成 */ 
	
}


/*-----------------------------------------------------------------------
	ADC1 MDA1模式设置
------------------------------------------------------------------------*/
void ADC1_DMA1_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);  		/* DMA channel1 configuration */ 

	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; 			//DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC1_ConvertedValue; 	//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 					//内存作为数据传输的目的地
	DMA_InitStructure.DMA_BufferSize = ADC1_SCAN_CHANNEL_NUM; 			//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 		//数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 			//工作在循环缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 		//DMA通道 x拥有高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 				//DMA通道x没有设置为内存到内存传输

	DMA_Init(DMA1_Channel1, &DMA_InitStructure); 				//根据DMA_InitStruct中指定的参数初始化DMA的通道

}



/*-----------------------------------------------------------------------
	ADC1的初始化
	注意:ADC的校准要在DMA之前，DMA的使能要在ADC之前，避免DMA数据错位
	相邻通道之间干扰问题?
------------------------------------------------------------------------*/
void ADC1_Init_All(void)
{

	ADC1_GPIO_Config();

	ADC1_DMA1_Config();

	/* 校准完成，ADC校准要在DMA使能之前 */ 	
	/*	
		原因：如果在初始化AD的时候DMA被触发了一次，但是此时并没有采样，但是DMA目的地址已经发生了自加，
		当你采样第一路的时候，数据却填充到了第二路。校准AD的时候会触发DMA导致通道错位，因此校准AD基准前不要启用DMA。
	*/
	ADC1_Mode_Config();	

	/* Enable DMA channel1,DMA使能要在ADC之前 */ 
	DMA_Cmd(DMA1_Channel1, ENABLE); 
	
 	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); 



}










