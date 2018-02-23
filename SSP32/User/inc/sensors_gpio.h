
#ifndef __SENSORS_GPIO_H
#define __SENSORS_GPIO_H

#ifndef  _SENSORS_GPIO_GLOBAL
#define  _SENSORS_GPIO_GLOBAL  extern 
#else
#define  _SENSORS_GPIO_GLOBAL 
#endif
#include "sys.h"














/* 2s */
//#define MOTION_CLEAR_TIME		200

/* 5s */
#define MOTION_CLEAR_TIME		500






void PM25_EXTI_init(void);




void PYD1798_gpio_init(void);
void PYD1798_readdigipyro_1(void);
void PYD1798_check(void);


void smokeModule_gpio_init(void);

void smokeModule_check(void);





#endif




















