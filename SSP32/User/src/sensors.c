
#define _SENSORS_GLOBAL

#include "includes.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#include "sensors.h"
#include "sensors_i2c_hal.h"






sensors_value_t sensors_value;

ADC1_value_t	ADC1_value;
sht3x_value_t 	sht3x_value;
	

/*-----------------------------------------------------------------------
	1us延迟
------------------------------------------------------------------------*/
void delay_1us(int32_t tt) 
{
	int32_t i;

	for (i=0; i<tt; i++){
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();

		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();

	}

}

//==============================================================================
void DelayMicroSeconds(int32_t nbrOfUs){   /* -- adapt this delay for your uC -- */
//==============================================================================
  int32_t i;
  for(i = 0; i < nbrOfUs; i++)
  {  
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop(); __nop(); __nop();
	  __nop(); __nop(); __nop();

		
  }
}




/*-----------------------------------------------------------------------
	每100ms执行一次
------------------------------------------------------------------------*/
void ADC1_value_filter(void)
{
	static int32_t filterFlag = 0;
	static ADC1_value_t filterData[ADC1_FILTER_NUM];
	int32_t i;

	filterData[filterFlag].co_tgs5342 = ADC1_ConvertedValue[0];
	filterData[filterFlag].voc_tgs2600 = ADC1_ConvertedValue[1];
	filterData[filterFlag].nis05 = ADC1_ConvertedValue[2];
	filterFlag++;

	if(filterFlag >= ADC1_FILTER_NUM){		/* 保存ADC1_FILTER_NUM 次数据后跟上一次的数据一起算平均 */

		for(i=1; i<ADC1_FILTER_NUM; i++){	/* 这里不直接使用ADC1_value，避免其他地方获取ADC1_value是出现差错 */
			filterData[0].co_tgs5342 += filterData[i].co_tgs5342;
			filterData[0].voc_tgs2600 += filterData[i].voc_tgs2600;
			filterData[0].nis05 += filterData[i].nis05;	
		}
		filterData[0].co_tgs5342 += ADC1_value.co_tgs5342;
		filterData[0].voc_tgs2600 += ADC1_value.voc_tgs2600;
		filterData[0].nis05 += ADC1_value.nis05; 

		ADC1_value.co_tgs5342 = filterData[0].co_tgs5342 / (ADC1_FILTER_NUM + 1);
		ADC1_value.voc_tgs2600 = filterData[0].voc_tgs2600 / (ADC1_FILTER_NUM + 1);
		ADC1_value.nis05 = filterData[0].nis05 / (ADC1_FILTER_NUM + 1);


		sensors_value.voc_tgs2600_RS_R0 = (4096.0/ADC1_value.voc_tgs2600 - 1.0) / VOC_TGS2600_R0;
		if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_1){
			sensors_value.voc_tgs2600_level = VOC_TGS2600_LEVEL_1;
			
		}else if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_2){
			sensors_value.voc_tgs2600_level = VOC_TGS2600_LEVEL_2;
			
		}else if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_3){
			sensors_value.voc_tgs2600_level = VOC_TGS2600_LEVEL_3;
			
		}else if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_4){
			sensors_value.voc_tgs2600_level = VOC_TGS2600_LEVEL_4;

		}else{
			sensors_value.voc_tgs2600_level = VOC_TGS2600_LEVEL_5;
		}

//		printf("\n co_tgs5342:%4d   voc_tgs2600:%4d   nis05:%4d", ADC1_value.co_tgs5342, ADC1_value.voc_tgs2600, ADC1_value.nis05);
		
//		printf("\n voc_tgs2600:%4d    RS_R0:%.03f   level:%d", ADC1_value.voc_tgs2600, sensors_value.voc_tgs2600_RS_R0, sensors_value.voc_tgs2600_level);

		filterFlag = 0;
		
	}


}




/*-----------------------------------------------------------------------
	
------------------------------------------------------------------------*/
void sensors_init_all(void)
{

	printf("\n 1. ADC1_Init_All.");
	ADC1_Init_All();

	printf("\n 2. GP2Y1023_pwm_init.");
	GP2Y1023_pwm_init();



	printf("\n 3. CO2_usart3_init.");
	CO2_usart3_init();


//	CO2_zero_config();
	CO2_atuo_config(1);
	CO2_setr_config(5000);

	printf("\n 4. PYD1798_gpio_init.");
 	PYD1798_gpio_init();


	printf("\n 5. SHT3X_Init(0x44).");
	SHT3X_Init(0x44);


}


/*-----------------------------------------------------------------------
	每2秒钟执行一次
------------------------------------------------------------------------*/
void SHT3X_read(void)
{
	if(SHT3X_ReadMeasurementBuffer(&sht3x_value.temperature, &sht3x_value.humidity) != NO_ERROR){
		sht3x_value.temperature = 0;
		sht3x_value.humidity = 0;
		
		SHT3X_SoftReset();
		SHT3X_Init(0x44);
	}
	
//	printf("\n sht3x t:%f h:%f", sht3x_value.temperature , sht3x_value.humidity);

}












