
#define _SENSORS_GPIO_GLOBAL

#include "includes.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
#include "stm32f10x.h"

#include "sensors.h"




// port A, bit 1           
#define PYD1798_OUT_LOW()  	(GPIOA->BRR = 0x00000002) 
#define PYD1798_OUT_HIGH() 	(GPIOA->BSRR = 0x00000002) 
#define PYD1798_IN_READ()  (GPIOA->IDR  & 0x0002)    


#define smokeModule_IN_READ()  (GPIOA->IDR  & 0x0020) 


/*-------------------------------------------------------------------------------------------


STM32��ÿ��IO�ڶ�������Ϊ�ж����룬Ҫ��IO����Ϊ�ⲿ�ж����룬��һ�¼������裺
��һ�� ��ʼ��IO����Ϊ����
	������Ҫ��Ϊ�ⲿ�ж������IO�ڵ�״̬����������Ϊ����/�������룬Ҳ��������Ϊ�������룬
���Ǹ��յ�ʱ��һ��Ҫ�����������������裬������ܵ����жϲ�ͣ�Ĵ������ڸ��Žϴ�ĵط���
����ʹ��������/������Ҳ����ʹ���ⲿ����/�������裬��������һ���̶ȷ�ֹ�ⲿ���Ŵ�����Ӱ�졣

�ڶ��� ����IO�ڸ���ʱ�ӣ�����IO�����ж��ߵ�ӳ���ϵ
	STM32��IO�����ж��ߵĶ�Ӧ��ϵ��Ҫ�����ⲿ�жϼĴ�����EXTICR��,��������Ҫ�ȿ�������ʱ�ӣ�
Ȼ������IO�����ж��ߵĶ�Ӧ��ϵ�����ܰ��ⲿ�ж����ж�������������

������ �������IO����Ե������ж�/�¼������ô�������
	����Ҫ�����жϵĲ�����������STM32�������ó������ش������½��ش��������������ƽ������
���ǲ������óɸߵ�ƽ���ߵ͵�ƽ��������������Լ���ʵ����������á�
ע���ڶ����������������Ϊһ������������

���Ĳ� �����жϷ��飨NVIC��,����ʹ���ж�
	��һ�������������жϷ��飬�Լ�ʹ�ܡ���STM32���ж���˵��ֻ��������NVIC�����ã����������ܱ�
ִ�У������ǲ���ִ�е��жϷ���������ȥ�ġ�

���岽 ��д�жϷ�����
	�����ж����õ����һ�����жϷ��������Ǳز����ٵģ���������￪�����жϣ�����û��д�жϷ���
�������Ϳ�������Ӳ�����󣬴Ӷ����³�������������ڿ�����ĳ���жϺ�һ��Ҫ�ǵ�Ϊ���жϱ�д����
���������жϷ������б�д��Ҫִ�е��жϺ�Ĳ�����

----------------------------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------
	1����ʼ������Ϊ��������
------------------------------------------------------------------------*/
void PM25_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;         		/* PA4 */                                   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//���Ƶ�����50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			//�������������

	GPIO_Init(GPIOA, &GPIO_InitStructure);
      
}

/*-----------------------------------------------------------------------
	2��ʹ��AFIO����ʱ�ӹ���
------------------------------------------------------------------------*/
void PM25_AFIO_config(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//ʹ��ʱ��

}

/*-----------------------------------------------------------------------
	3����GPIO�����ж���ӳ������ 
	�������IO����Ե������ж�/�¼������ô�������
------------------------------------------------------------------------*/
void PM25_EXTILine_config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	 
	EXTI_ClearITPendingBit(EXTI_Line4);		//����жϱ�־

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);		//ѡ���жϹܽ�PA4
	 
	EXTI_InitStructure.EXTI_Line = EXTI_Line4; 						//ѡ���ж���·4
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 			//����Ϊ�ж����󣬷��¼�����
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 	//�����жϴ�����ʽΪ���½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//�ⲿ�ж�ʹ��
	
	EXTI_Init(&EXTI_InitStructure);

}

/*-----------------------------------------------------------------------
	4�������жϷ��飨NVIC��,����ʹ���ж�
------------------------------------------------------------------------*/
void PM25_NVIC_config(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;	//����ṹ��

//	NVIC_Configuration();	//ǰ���Ѿ��������жϷ���2

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; 		//ʹ���ⲿ�ж����ڵ�ͨ��

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ� 2�� 

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 	//�����ȼ� 2

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 	//ʹ���ⲿ�ж�ͨ�� 

	NVIC_Init(&NVIC_InitStructure); 		//���ݽṹ����Ϣ�������ȼ���ʼ�� 


}







/*-----------------------------------------------------------------------
	5���жϷ�����
------------------------------------------------------------------------*/
void EXTI4_IRQHandler(void)
{
	
	sensors_printf("\n EXTI4_IRQHandler");
	   
	EXTI_ClearITPendingBit(EXTI_Line4);	//��� LINE �ϵ��жϱ�־λ
	
}


/*-----------------------------------------------------------------------
	��ʼ��
	ʹ���ⲿ�жϷ�����������pm2.5���������Ϊ10ms������ʱ��̫��
	ʹ���ⲿ�жϴ���������������ʵ�ָ߾���
	�ʴ˷����ݲ�ʹ��
------------------------------------------------------------------------*/
void PM25_EXTI_init(void)
{
	PM25_gpio_config();
	PM25_AFIO_config();
	PM25_EXTILine_config();
	PM25_NVIC_config();
}



/*-----------------------------------------------------------------------
	PYD1798
------------------------------------------------------------------------*/
void PYD1798_gpio_init(void)
{

	RCC->APB2ENR |= 0x00000004;  // I/O port A clock enabled

	GPIOA->CRL &= 0xFFFFFF0F;  // set output 
	GPIOA->CRL |= 0x00000010;  // 

}






/*-----------------------------------------------------------------------
	PYD1798
------------------------------------------------------------------------*/
void PYD1798_pin_out(void)
{

	GPIOA->CRL &= 0xFFFFFF0F;  
	GPIOA->CRL |= 0x00000010;  // 

}

/*-----------------------------------------------------------------------
	PYD1798
------------------------------------------------------------------------*/
void PYD1798_pin_in(void)
{

	GPIOA->CRL &= 0xFFFFFF0F;  
	GPIOA->CRL |= 0x00000080;  // 

}


/*-----------------------------------------------------------------------
	�ж��Ƿ��������ƶ�
	����10�����ݵ�����ݼ���ʾ�������ƶ�������2s
	������2s��û�м�⵽һ������10�ε�����ݼ�����������ƶ���־
------------------------------------------------------------------------*/
void remove_check(int chl)
{
	static int old_chl = 0;		/* ǰһ�ε�chl����  */	
	static int dir = 0;			/* ǰһ�Ƚ��ǵ��� 1�����ǵݼ� 0  */
	static int count = 0;		/* ����������ݼ�����  */
	static int uncount = 0;		/* �������ٴ�û�м�⵽�����ƶ�  */


	if(dir){					/* ǰһ�ε��� */
		if(chl > old_chl){		/* ���� */
			count++;
		}else{
			if(chl < old_chl){	/* �ݼ� */
				count = 1;
				dir = 0;
			}else{
				count = 0;	
			}
		}

	}else{

		if(chl < old_chl){		/* �ݼ� */
			count++;
		}else{
			if(chl > old_chl){	/* ���� */
				count = 1;
				dir = 1;
			}else{
				count = 0;	
			}
		}

	}

	old_chl = chl;				/* �������� */

	
	if(count >= 10){					/* ����10�����ϵ�����ݼ� */
		sensors_value.removeFlag = 1;	/* ��⵽�����ƶ� */
		uncount = 0;	
	}else{
		uncount++;
	}

	if((sensors_value.removeFlag == 1) && uncount > 200){	/* ����200��û�м�⵽�����ƶ� */
		sensors_value.removeFlag = 0;	/* ȡ�������ƶ���־ */

	}


}





/*-----------------------------------------------------------------------
	PYD1798
------------------------------------------------------------------------*/
void PYD1798_readdigipyro(void)
{
	int i, k, PIRval[3] = {0} ;	
	uint bitmask;
	
	PYD1798_pin_out(); 
	PYD1798_OUT_HIGH();

	delay_1us(150);
	for(k=0; k<2; k++) // get 14bits of DL data for each channel
	{
		bitmask = 0x2000; // Set BitPos
		PIRval[k] = 0;
		for (i=0; i < 14; i++)
		{
			// create low to high transition			
			PYD1798_OUT_LOW(); 	// Set DL = Low, Low level duration must be > 200 ns (tL)

			delay_1us(1);		// number of nop to be adapted to processor speed
			PYD1798_OUT_HIGH(); // Set DL = High, High level duration must be > 200 ns (tH)
			delay_1us(1); 

			PYD1798_pin_in(); 	// Configure DL as Input
			delay_1us(8); 		// Wait for stable signal
			if (PYD1798_IN_READ()) PIRval[k] |= bitmask; // If DL High set masked bit in PIRVal
			bitmask >>= 1;
			
			PYD1798_OUT_LOW();
			PYD1798_pin_out(); 
		}
		
	}
	
//	sensors_printf(" [%d](%d)(%d)", PIRval[0], PIRval[1], (PIRval[1] -6700)/80 + 25);
//	sensors_printf(" %d", PIRval[0]);

	PYD1798_OUT_LOW();  // Set DL = Low
	delay_1us(1);	

	PYD1798_pin_in(); 	 // Configure DL as Input
	
	remove_check(PIRval[0]);	/* ��������ƶ� */

}

/*-----------------------------------------------------------------------
	PYD1798
	ÿ2msִ��һ��,PYD1798ÿ10ms��ȡһ��
------------------------------------------------------------------------*/
void PYD1798_check(void)
{
	static int i = 0;

	i++;
	if(i >= 5){
		PYD1798_readdigipyro();
		i = 0;
	}

}


/*-----------------------------------------------------------------------
	�������ų�ʼ��
------------------------------------------------------------------------*/
void smokeModule_gpio_init(void)
{

	RCC->APB2ENR |= 0x00000004;  	// I/O port A clock enabled

	GPIOA->CRL &= 0xFF0FFFFF;  	// set PA5 INPUT
	GPIOA->CRL |= 0x00800000;  	// 

}

/*-----------------------------------------------------------------------
	������
	ÿ2msִ��һ��
------------------------------------------------------------------------*/
void smokeModule_check(void)
{	
	if(smokeModule_IN_READ()){		
		sensors_value.smokeModule_value = 1;		
	}else{	
		sensors_value.smokeModule_value = 0;		
	}

}







