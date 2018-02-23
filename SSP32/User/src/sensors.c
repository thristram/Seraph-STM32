
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


/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void sensors_CO2_power_init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(CO2_RCC_CLOCK, ENABLE);	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Pin = CO2_GPIO_PIN;	    		
	GPIO_Init(CO2_GPIO_BASE, &GPIO_InitStructure);	  		

//	CO2_POWER_OFF;
	CO2_POWER_ON;
}

/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void sensors_CO_power_init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(CO_RCC_CLOCK, ENABLE);	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Pin = CO_GPIO_PIN;	    		
	GPIO_Init(CO_GPIO_BASE, &GPIO_InitStructure);	  		

//	CO_POWER_OFF;
	CO_POWER_ON;
}

/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void sensors_VOC_power_init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(VOC_RCC_CLOCK, ENABLE);	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Pin = VOC_GPIO_PIN;	    		
	GPIO_Init(VOC_GPIO_BASE, &GPIO_InitStructure);	  		

//	VOC_POWER_OFF;
	VOC_POWER_ON;
	
}

/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void sensors_PM25_power_init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(PM25_RCC_CLOCK, ENABLE);	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Pin = PM25_GPIO_PIN;	    		
	GPIO_Init(PM25_GPIO_BASE, &GPIO_InitStructure);	  		

//	PM25_POWER_OFF;
	
	PM25_POWER_ON;
}

/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void sensors_FAN1_power_init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(FAN1_RCC_CLOCK, ENABLE);	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Pin = FAN1_GPIO_PIN;	    		
	GPIO_Init(FAN1_GPIO_BASE, &GPIO_InitStructure);	  		

//	FAN1_POWER_OFF;
	FAN1_POWER_ON;
}

/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void sensors_FAN2_power_init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(FAN2_RCC_CLOCK, ENABLE);	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Pin = FAN2_GPIO_PIN;	    		
	GPIO_Init(FAN2_GPIO_BASE, &GPIO_InitStructure);	  		

//	FAN2_POWER_OFF;	
	FAN2_POWER_ON;
}

/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void buzzer_power_init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(BUZZER_RCC_CLOCK, ENABLE);	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;	    		
	GPIO_Init(BUZZER_GPIO_BASE, &GPIO_InitStructure);	  		

	BUZZER_POWER_OFF;
}


/*-----------------------------------------------------------------------
	20次平滑
------------------------------------------------------------------------*/
void voc_ppm_smooth(int voc)
{
	static u8 voc_buf[VOC_PPM_BUF_LEN] = {0};
	static u8 voc_cnt = 0;
	u8 	i;
	u16 	sum = 0;

	if(voc > 100){voc = 100;}

	if(voc_cnt < VOC_PPM_BUF_LEN){
		
		voc_buf[voc_cnt] = (u8)voc;
		voc_cnt++;

	}else{

		for(i = 0; i < VOC_PPM_BUF_LEN - 1; i++){			
			voc_buf[i] = voc_buf[i + 1];
		}
		
		voc_buf[VOC_PPM_BUF_LEN -1] = (u8)voc;

	}


	for(i = 0; i < voc_cnt; i++){			
		sum += voc_buf[i];
	}

	sensors_value.voc_ppm = sum / voc_cnt;

}





/*-----------------------------------------------------------------------
	每300ms执行一次
------------------------------------------------------------------------*/
void ADC1_value_filter(void)
{
	static u8 filterFlag = 0;
	static ADC1_value_t filterData[ADC1_FILTER_NUM];
	int i, vod_ppm = 0;

	filterData[filterFlag].co_tgs5342 = ADC1_ConvertedValue[0];
	filterData[filterFlag].voc_tgs2600 = ADC1_ConvertedValue[1];
//	filterData[filterFlag].nis05 = ADC1_ConvertedValue[2];
	filterFlag++;

	if(filterFlag >= ADC1_FILTER_NUM){		/* 保存ADC1_FILTER_NUM 次数据后跟上一次的数据一起算平均 */

		for(i=1; i<ADC1_FILTER_NUM; i++){	/* 这里不直接使用ADC1_value，避免其他地方获取ADC1_value是出现差错 */
			filterData[0].co_tgs5342 += filterData[i].co_tgs5342;
			filterData[0].voc_tgs2600 += filterData[i].voc_tgs2600;
//			filterData[0].nis05 += filterData[i].nis05;	
		}
		filterData[0].co_tgs5342 += ADC1_value.co_tgs5342;
		filterData[0].voc_tgs2600 += ADC1_value.voc_tgs2600;
//		filterData[0].nis05 += ADC1_value.nis05; 

//		ADC1_value.co_tgs5342 = filterData[0].co_tgs5342 / (ADC1_FILTER_NUM + 1);
		ADC1_value.voc_tgs2600 = filterData[0].voc_tgs2600 / (ADC1_FILTER_NUM + 1);
//		ADC1_value.nis05 = filterData[0].nis05 / (ADC1_FILTER_NUM + 1);


		sensors_value.voc_tgs2600_RS_R0 = ((4096.0/ADC1_value.voc_tgs2600) * 5 /3.3 - 1) * 10  / VOC_TGS2600_R0;

//		sensors_value.voc_tgs2600_RS_R0 = (4096.0/ADC1_value.voc_tgs2600 - 1.0) / VOC_TGS2600_R0;


		if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_9){		//0-8
			
			if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_5){		//0-4
			
				if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_2){ 			// 1
				
					vod_ppm = 1;
					
				}else if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_3){		//10-13
				
					vod_ppm = 2;
					
				}else if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_4){		//10-13

					vod_ppm = 3;
					
				}else{													//14 - 19
					vod_ppm = 4;
			
				}				

			}else{												//5-8

				vod_ppm = 5 + (int)((VOC_TGS2600_RS_R0_5 -sensors_value.voc_tgs2600_RS_R0) / 0.02);
			}
			
		}else{

			if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_20){		//9-19
				
				if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_10){ 		//9
				
					vod_ppm = 9;
					
				}else if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_14){		//10-13
				
					vod_ppm = 10 + (int)((VOC_TGS2600_RS_R0_10 -sensors_value.voc_tgs2600_RS_R0) / 0.0075);
					
				}else{													//14 - 19
					vod_ppm = 14 + (int)((VOC_TGS2600_RS_R0_14 -sensors_value.voc_tgs2600_RS_R0) / 0.005);
			
				}					
			}
			else{												//20-80---
			
				if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_40){ 			//20 - 39
				
					vod_ppm = 20 + (int)((VOC_TGS2600_RS_R0_20 -sensors_value.voc_tgs2600_RS_R0) / 0.003);
											
				}else if(sensors_value.voc_tgs2600_RS_R0 > VOC_TGS2600_RS_R0_50){		//10-13
				
					vod_ppm = 40 + (int)((VOC_TGS2600_RS_R0_40 -sensors_value.voc_tgs2600_RS_R0) / 0.002);

				}else{
			
					vod_ppm = 50 + (int)((VOC_TGS2600_RS_R0_50 -sensors_value.voc_tgs2600_RS_R0) / 0.001);
								
				}											
			}
			
		}
		
		voc_ppm_smooth(vod_ppm);

		filterFlag = 0;
		
	}


}




/*-----------------------------------------------------------------------
	
------------------------------------------------------------------------*/
void sensors_init_all(void)
{
	sensors_CO2_power_init();
	sensors_CO_power_init();
	sensors_VOC_power_init();
	sensors_PM25_power_init();
	sensors_FAN1_power_init();
	sensors_FAN2_power_init();
	buzzer_power_init();

	/* memset 是否可用? */
	memset(&sensors_value, 0, sizeof(sensors_value));
	sensors_value.removeFlag = 0;

	sensors_printf("\n 1. ADC1_Init_All.");
	ADC1_Init_All();		/* VOC CO  */

	sensors_printf("\n 2. GP2Y1023_pwm_init.");
	GP2Y1023_pwm_init();	/* PM2.5 */

	sensors_printf("\n 3. smokeModule_gpio_init.");
	smokeModule_gpio_init();/* 烟雾 */

	sensors_printf("\n 4. CO2_usart3_init.");
	CO2_usart3_init();		/* 初始化 */
//	CO2_zero_config();
	CO2_atuo_config(1);		/* 配置自动校零 */
	CO2_setr_config(5000);	/* 配置量程 */

	CO_init();

	sensors_printf("\n 5. PYD1798_gpio_init.");
 	PYD1798_gpio_init();	/* 人体移动 */


	sensors_printf("\n 6. SHT3X_Init(0x44).");
	SHT3X_Init(0x44);		/* sht30 */



}


/*-----------------------------------------------------------------------
	每2秒钟执行一次
------------------------------------------------------------------------*/
void SHT3X_read(void)
{
	if(SHT3X_ReadMeasurementBuffer(&sensors_value.sht3x.temperature, &sensors_value.sht3x.humidity) != NO_ERROR){

		sensors_value.sht3x.temperature = 0;
		sensors_value.sht3x.humidity 	= 0;
		sensors_value.sht3x.errCnt++;
		if(sensors_value.sht3x.errCnt >= 3){			/* 连续三次读取错误 */
			sensors_value.sht3x.errCnt = 3;
		}		
		SHT3X_SoftReset();
		SHT3X_Init(0x44);
		
	}else{
		sensors_value.sht3x.errCnt = 0;
		
	}
	
}












