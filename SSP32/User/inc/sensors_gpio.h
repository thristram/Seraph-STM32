
#ifndef __SENSORS_GPIO_H
#define __SENSORS_GPIO_H

#ifndef  _SENSORS_GPIO_GLOBAL
#define  _SENSORS_GPIO_GLOBAL  extern 
#else
#define  _SENSORS_GPIO_GLOBAL 
#endif
#include "sys.h"



















extern int smokeModule_value;






void PM25_EXTI_init(void);




void PYD1798_gpio_init(void);
void PYD1798_readdigipyro(void);
void PYD1798_check(void);



void smokeModule_check(void);





#endif




















