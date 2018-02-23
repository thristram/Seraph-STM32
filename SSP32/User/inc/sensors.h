
#ifndef __SENSORS_H
#define __SENSORS_H

#ifndef  _SENSORS_GLOBAL
#define  _SENSORS_GLOBAL  extern 
#else
#define  _SENSORS_GLOBAL 
#endif

#include "sys.h"
#include "sensors_adc.h"
#include "sensors_timer.h"
#include "sensors_gpio.h"
#include "sensors_uart.h"
#include "sensors_typedefs.h"
#include "sensors_sht3x.h"



typedef unsigned char   uchar;                                  
typedef unsigned short  ushort;     
typedef unsigned int  	uint;    





#define PRINTF_EN		0

#if PRINTF_EN

#define sensors_printf(...)   		printf( __VA_ARGS__ )


#else

#define sensors_printf(...) 		

#endif



#define CO2_RCC_CLOCK			RCC_APB2Periph_GPIOC
#define CO2_GPIO_BASE			GPIOC
#define CO2_GPIO_PIN				GPIO_Pin_7
#define CO2_POWER_ON			(CO2_GPIO_BASE->BSRR = CO2_GPIO_PIN)
#define CO2_POWER_OFF			(CO2_GPIO_BASE->BRR = CO2_GPIO_PIN)

#define CO_RCC_CLOCK				RCC_APB2Periph_GPIOC
#define CO_GPIO_BASE				GPIOC
#define CO_GPIO_PIN				GPIO_Pin_9
#define CO_POWER_ON				(CO_GPIO_BASE->BSRR = CO_GPIO_PIN)
#define CO_POWER_OFF			(CO_GPIO_BASE->BRR = CO_GPIO_PIN)

#define VOC_RCC_CLOCK			RCC_APB2Periph_GPIOC
#define VOC_GPIO_BASE			GPIOC
#define VOC_GPIO_PIN				GPIO_Pin_0
#define VOC_POWER_ON			(VOC_GPIO_BASE->BSRR = VOC_GPIO_PIN)
#define VOC_POWER_OFF			(VOC_GPIO_BASE->BRR = VOC_GPIO_PIN)

#define PM25_RCC_CLOCK			RCC_APB2Periph_GPIOC
#define PM25_GPIO_BASE			GPIOC
#define PM25_GPIO_PIN			GPIO_Pin_8
#define PM25_POWER_ON			(PM25_GPIO_BASE->BSRR = PM25_GPIO_PIN)
#define PM25_POWER_OFF			(PM25_GPIO_BASE->BRR = PM25_GPIO_PIN)

#define FAN1_RCC_CLOCK			RCC_APB2Periph_GPIOC
#define FAN1_GPIO_BASE			GPIOC
#define FAN1_GPIO_PIN			GPIO_Pin_2
#define FAN1_POWER_ON			(FAN1_GPIO_BASE->BSRR = FAN1_GPIO_PIN)
#define FAN1_POWER_OFF			(FAN1_GPIO_BASE->BRR = FAN1_GPIO_PIN)

#define FAN2_RCC_CLOCK			RCC_APB2Periph_GPIOC
#define FAN2_GPIO_BASE			GPIOC
#define FAN2_GPIO_PIN			GPIO_Pin_3
#define FAN2_POWER_ON			(FAN2_GPIO_BASE->BSRR = FAN2_GPIO_PIN)
#define FAN2_POWER_OFF			(FAN2_GPIO_BASE->BRR = FAN2_GPIO_PIN)

#define BUZZER_RCC_CLOCK			RCC_APB2Periph_GPIOB
#define BUZZER_GPIO_BASE			GPIOB
#define BUZZER_GPIO_PIN			GPIO_Pin_1
#define BUZZER_POWER_ON			(BUZZER_GPIO_BASE->BSRR = BUZZER_GPIO_PIN)
#define BUZZER_POWER_OFF		(BUZZER_GPIO_BASE->BRR = BUZZER_GPIO_PIN)


/* 烟雾报警 */
#define SMOG_ALARM_ON			BUZZER_POWER_ON
#define SMOG_ALARM_OFF			BUZZER_POWER_OFF





typedef struct
{
	u32	co_tgs5342;
	u32	voc_tgs2600;
	u32 nis05;
	

}ADC1_value_t;



typedef enum 
{
	VOC_TGS2600_LEVEL_1 = 1,
	VOC_TGS2600_LEVEL_2 = 2,
	VOC_TGS2600_LEVEL_3,
	VOC_TGS2600_LEVEL_4,
	VOC_TGS2600_LEVEL_5,

}VOC_TGS2600_LEVEL;



typedef struct
{
	u32t	  serialNumber;// serial number
	regStatus status;	   // sensor status
	ft		  temperature; // temperature [癈]
	ft		  humidity;    // relative humidity [%RH]
	bt		heater; 	 // heater, false: off, true: on

	u8		errCnt; 		/* 通讯故障检测 */


}sht3x_value_t;





typedef struct
{

	int		co_ppm;					/* co浓度 0.1ppm */
	int		co_range;				/* co量程 0.1ppm */
	int		co_recv_cnt;				/* co接收计数 */


	float 	voc_tgs2600_RS_R0;				/* RS/R0 R0 = */
	int	voc_ppm;	


	float	pm25_density;			/* pm2.5浓度值 */


	int 	co2_density;			/* 读取的co2浓度值  */
	int	co2_recv_cnt;			/* co2接收计数 */


	u8 	removeFlag;				/* PYD1798 为1是表示检测到人体移动，为0是表示没有检测到人体移动 */

	u8 	smokeModule_value;		/* 烟雾 */

	

	sht3x_value_t sht3x;		/* sht30 */




}sensors_value_t;






/*--------------------------- ADC1 ---------------------------------*/


#define ADC1_FILTER_NUM 		3


/*------------------------ VOC_TGS2600 -------------------------------*/

/* 2600在空气中的电阻值 kΩ 对应的电压采样值为195，室内平均采样值为250  */
//#define VOC_TGS2600_R0		20.0	
#define VOC_TGS2600_R0		50.0	


//#define VOC_TGS2600_RS_R0_1	0.5
//#define VOC_TGS2600_RS_R0_2	0.3
//#define VOC_TGS2600_RS_R0_3	0.2
//#define VOC_TGS2600_RS_R0_4	0.15

#define VOC_TGS2600_RS_R0_1		0.70
#define VOC_TGS2600_RS_R0_2		0.56
#define VOC_TGS2600_RS_R0_3		0.49
#define VOC_TGS2600_RS_R0_4		0.45
#define VOC_TGS2600_RS_R0_5		0.42
#define VOC_TGS2600_RS_R0_9		0.34
#define VOC_TGS2600_RS_R0_10	0.33
#define VOC_TGS2600_RS_R0_14	0.30
#define VOC_TGS2600_RS_R0_20	0.27
#define VOC_TGS2600_RS_R0_40	0.21
#define VOC_TGS2600_RS_R0_50	0.19

#define VOC_PPM_BUF_LEN			20




extern ADC1_value_t	ADC1_value;
extern sensors_value_t sensors_value;




void delay_1us(int32_t tt);

void DelayMicroSeconds(int32_t nbrOfUs);




void ADC1_value_filter(void);

void sensors_init_all(void);

void SHT3X_read(void);



#endif




















