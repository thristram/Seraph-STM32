
#ifndef __SENSORS_TIMER_H
#define __SENSORS_TIMER_H

#ifndef  _SENSORS_TIMER_GLOBAL
#define  _SENSORS_TIMER_GLOBAL  extern 
#else
#define  _SENSORS_TIMER_GLOBAL 
#endif

#include "sys.h"









typedef struct
{
	uint16_t 	IC2Value;
	uint16_t 	IC1Value;

	uint32_t 	timerFreq;
	uint32_t 	frequency;

	float 		dutyCycle;

}GP2Y1023_value_t;









extern GP2Y1023_value_t GP2Y1023_value;







void GP2Y1023_pwm_init(void);





#endif




















