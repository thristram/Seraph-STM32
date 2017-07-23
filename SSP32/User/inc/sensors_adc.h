
#ifndef __SENSORS_ADC_H
#define __SENSORS_ADC_H

#ifndef  _SENSORS_ADC_GLOBAL
#define  _SENSORS_ADC_GLOBAL  extern 
#else
#define  _SENSORS_ADC_GLOBAL 
#endif

#include "sys.h"





#define ADC1_SCAN_CHANNEL_NUM	2


extern __IO uint16_t ADC1_ConvertedValue[ADC1_SCAN_CHANNEL_NUM];



void ADC1_Init_All(void);



#endif




















