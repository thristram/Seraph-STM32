
#ifndef __SENSORS_TIMER_H
#define __SENSORS_TIMER_H

#ifndef  _SENSORS_TIMER_GLOBAL
#define  _SENSORS_TIMER_GLOBAL  extern 
#else
#define  _SENSORS_TIMER_GLOBAL 
#endif

#include "sys.h"






#define OUTPUT_AT_NO_DUST		1350		/* 无尘时输出低电平时间 */
//#define OUTPUT_AT_NO_DUST		1400		/* 无尘时输出低电平时间 */

#define OUTPUT_AT_MAX_DUST		5200		/* 最大输出低电平时间 */
#define OUTPUT_SENSITIVITY_K		3.33		/* 乘积因素 */

#define PM25_BUF_LEN				20		


typedef struct
{
	uint16_t 	IC2Value;
	uint16_t 	IC1Value;

	uint32_t 	timerFreq;

	int 		output_us;		/* 低电平的us数 */
	int 		com_cnt;			/* 通讯计数每个周期加1，即每10ms加1 */
	
	float 		frequency;
	float 		dutyCycle;

}GP2Y1023_value_t;









extern GP2Y1023_value_t GP2Y1023_value;







void GP2Y1023_pwm_init(void);





#endif




















