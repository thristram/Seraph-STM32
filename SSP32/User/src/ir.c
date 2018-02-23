#include "includes.h"
#include "stm32f10x.h"
#include "includes.h"
#include "key_string.h"

#include "ir.h"


u8 use;


IR_frame_t IR_frame;






/*----------------------------------------------------------------------------
	0x01-0x7f
-----------------------------------------------------------------------------*/
u8 ir_get_message_id(void)
{
	static u8 id = 0;
	
	id++;

	if(id >= 0x80){
		id = 1;
	}

	return id;
}


#if 1

/*-------------------------------------------------------------------------
     PA8 PA8/USART1_CK/TIM1_CH1/
-------------------------------------------------------------------------*/
void GPIO_PWM_Config(void)  
{ 
	GPIO_InitTypeDef GPIO_InitStructure; 

	/* GPIOA and GPIOB clock enable */ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  

	/*GPIOA Configuration: TIM1 channel 1 as alternate function push-pull */ 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           // ����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
} 

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void TIM1_PWM_Enable(void)
{    	
	TIM1->CR1 |= TIM_CR1_CEN;
}  

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void TIM1_PWM_Disable(void)
{    	
	TIM1->CNT = 10;			//ʹpwm����ǰ����͵�ƽ
	TIM1->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));

}  

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void TIM1_PWM_set_freq(u16 fre_kHz)
{    	
//	TIM_SetAutoreload(TIM1, (1000 / fre_kHz) -1); 
	
	TIM1->ARR = (1000 / fre_kHz) -1;

}  


/*-------------------------------------------------------------------------

 // �����е�����Ҫ�ı�pwm��Ƶ�ʺ�ռ�ձȵ��ã�
 TIM_SetAutoreload(TIM1,1000); 
 TIM_SetCompare1(TIM1,500); 
 
-------------------------------------------------------------------------*/ 
void TIM1_PWM_Config(void)  
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
	TIM_OCInitTypeDef  TIM_OCInitStructure;  
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

	/* PWM�źŵ�ƽ����ֵ */  
	u16 CCR1= 9;          

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/* Time base configuration */                                            
	TIM_TimeBaseStructure.TIM_Period = 25;  						//ʹ��25ʱ���������Ϊ26us��38k
	TIM_TimeBaseStructure.TIM_Prescaler = 72 -1;                                    //1MHz  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                	//����ʱ�ӷ�Ƶϵ��������Ƶ  
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;               //���ϼ������ģʽ  
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;              		 

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);  

	/* PWM1 Mode configuration: Channel1 */  
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //����ΪPWMģʽ1  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
	TIM_OCInitStructure.TIM_Pulse = CCR1;                                       	//��������ֵ�������������������ֵʱ����ƽ��������  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                    //����ʱ������ֵС��CCR1ʱΪ�ߵ�ƽ  

	//���漸�������Ǹ߼���ʱ���Ż��õ���ͨ�ö�ʱ����������		   
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;		 //���û������������		  
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;	//ʹ�ܻ��������		  
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		  //���������״̬		   
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;		//�����󻥲������״̬		   

	//����ѡ����ͨ��CH1,ʹ��TIM1_CH1ͨ��
	TIM_OC1Init(TIM1, &TIM_OCInitStructure); 										   //����ָ��������ʼ��  
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  

	//���岽��������ɲ���������ã��߼���ʱ�����еģ�ͨ�ö�ʱ���������� 		
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;//����ģʽ�����
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;//����ģʽ�����ѡ��		   
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;		  //��������		
	TIM_BDTRInitStructure.TIM_DeadTime = 0; 										//����ʱ������		   
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;				  //ɲ������ʹ��		 
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;//ɲ�����뼫��		
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//�Զ����ʹ��			
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);                                         //ʹ��TIM1���ؼĴ���ARR  

	/* TIM3 enable counter */  
	TIM_Cmd(TIM1, ENABLE);                                                      //ʹ��TIM3   

	TIM_CtrlPWMOutputs(TIM1, ENABLE);                                   //pwm���ʹ�ܣ�һ��Ҫ�ǵô�

} 


/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void TIM4_config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 9000;			//
	TIM_TimeBaseStructure.TIM_Prescaler = 72 -1;		//1MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM4, DISABLE);	//Ĭ�Ϲر�
		 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void TIM4_set_wait_time(u16 us)
{
	/* Set the Autoreload value */
	TIM4->ARR = us;

	TIM4->CNT = 0;			//��������

	/* Clear the flags */
	TIM4->SR = (u16)~TIM_IT_Update;
	
   	 /* Enable the TIM Counter */
	TIM4->CR1 |= TIM_CR1_CEN;
	 
}

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void IR_init(void)  
{  
	//PA8��Ϊ���ⷢ�ͽӿ�
	GPIO_PWM_Config();
	
	//tim1 ��Ϊ����pwm�ز��Ķ�ʱ��
	TIM1_PWM_Config();

	//tim4 ���ƺ��ⷢ�͵ı�����Ϣ
	TIM4_config();


}


/*-------------------------------------------------------------------------
    �������붨ʱ��4��ʹ�ö�ʱ�������жϺ�ʼ����
-------------------------------------------------------------------------*/
void IR_send_begin(void)
{
	/* Set the Autoreload value */
	TIM4->ARR = 100;		//�������ֵ
	TIM4->CNT = 0;			//��������

	/* Clear the flags */
	TIM4->SR = (u16)~TIM_IT_Update;
	
   	 /* Enable the TIM Counter */
	TIM4->CR1 |= TIM_CR1_CEN;
	 
}

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);





		
	}

}

#endif



void IR_test(void)  
{  
	static u8 i = 0;
	


	i++;

	if(i > 20){		//20s

		ssp_action_learn_ir();
		i = 0;

	}

//	use = mem_perused();
}



/*-----------------------------------------------------------------------
	1������ʱ��
------------------------------------------------------------------------*/
void IR_Module_RCC_config(void)
{

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_AFIO, ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

}



/*-----------------------------------------------------------------------
	2�������ж�
------------------------------------------------------------------------*/
void IR_Module_NVIC_config(void) 
{ 
	NVIC_InitTypeDef NVIC_InitStructure;
 
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;	//��ռ���ȼ�
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//�����ȼ�
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
  	NVIC_Init(&NVIC_InitStructure);		//����ָ���Ĳ�����ʼ��VIC�Ĵ���

}


/*-----------------------------------------------------------------------
	3����ʼ������Ϊ
------------------------------------------------------------------------*/
void IR_Module_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure); 		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
  
}


/*-----------------------------------------------------------------------
	4��IR_Module_uart4_config
------------------------------------------------------------------------*/
void IR_Module_uart4_config(void)
{
  	USART_InitTypeDef USART_InitStructure;			//����һ���������ڲ����Ľṹ��
  
  	USART_DeInit(UART4);  

	USART_InitStructure.USART_BaudRate = 19200; 		//������9600
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8λ����λ
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//1λֹͣλ
  	USART_InitStructure.USART_Parity = USART_Parity_No;			//��У��
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//��������ģʽ
  	
	USART_Init(UART4, &USART_InitStructure); //��ʼ������
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4, ENABLE);    

}


/*-----------------------------------------------------------------------
	CO��ʼ��
------------------------------------------------------------------------*/
void IR_Module_init(void)
{	
	IR_Module_RCC_config();
	IR_Module_NVIC_config();
	IR_Module_gpio_config();
	IR_Module_uart4_config();
}





/*----------------------------------------------------------------------------
	ssp��recepit�ظ�
-----------------------------------------------------------------------------*/
void ir_recepit_response(int code, u16 msgid)
{
	cJSON *cs;
	char *payload;
	char *topic = "";
	u8	ret = 0x83;
	
	cs = cJSON_CreateObject();	
	cJSON_AddNumberToObject(cs, key_code, code);

	if(code == RECEIPT_CODE_SUCCESS){
		cJSON_AddStringToObject(cs, key_msg, key_0x0200000);
	}
	
	if(code == RECEIPT_CODE_ANALYZE_OK){
		cJSON_AddStringToObject(cs, key_msg, key_0x0200001);
		ret = 0x85;
	}

	if(code == RECEIPT_CODE_FAILED){
		cJSON_AddStringToObject(cs, key_msg, key_0x0400000);
		ret = 0x85;
	}


	if(code == RECEIPT_CODE_ERROR){
		cJSON_AddStringToObject(cs, key_msg, key_0x0401F08);
		ret = 0x85;
	}

	payload = cJSON_PrintUnformatted(cs);
	if(payload){
		ssp_send_message(ret, 1, 0, topic, msgid, 0, payload);
		myfree(payload);
	}	
	cJSON_Delete(cs);

	
}



/*----------------------------------------------------------------------------
	�ϱ�irѧϰ���
-----------------------------------------------------------------------------*/
void ir_learn_post_result(void)
{
	cJSON *cs, *report, *ir, *raw;
	char *payload = NULL;
	char *topic = "/rt";
	u16 i, num = 0;

	cs = cJSON_CreateObject();    
	if(cs){
		
		report = cJSON_CreateObject();	
		if(report){
			cJSON_AddStringToObject(report, "SEPID", ss.deviceid);
			cJSON_AddStringToObject(report, key_type, "IR");	
			cJSON_AddNumberToObject(report, key_value, IR_frame.irbuf[IR_POSITION_TYPE]);
			cJSON_AddItemToObject(cs, key_report, report);
		}
		

		if(IR_frame.irbuf[IR_POSITION_TYPE] ==  1){		//ѧϰ�ɹ�
		
			ir = cJSON_CreateObject();	
			if(ir){
				cJSON_AddNumberToObject(ir, key_type, 0);
				
				raw = cJSON_CreateArray();
				if(raw){
					num = IR_frame.rxlen -5;
					if((num > 0) && (num	% 2 == 0)){
						num = num /2;				//ѧϰ���bit����						
						for(i = 0; i < num; i++){								
							cJSON_AddItemToArray(raw, cJSON_CreateNumber(IR_frame.irbuf[IR_POSITION_DATA + i * 2] * 256 + IR_frame.irbuf[IR_POSITION_DATA + 1 + i * 2]));												
						}
					}
					cJSON_AddItemToObject(ir, "raw", raw);
				}
				cJSON_AddItemToObject(cs, "IR", ir);
			}

		}
		

		payload = cJSON_PrintUnformatted(cs);
		if(payload){
			ssp_send_message(0x43, 1, 3, topic, ssp_get_message_id(), 0, payload);	
			
//			use = mem_perused();
			myfree(payload);
		}	
		cJSON_Delete(cs);
		
	}

}



/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void IR_recv_handle(void)
{

	if(IR_frame.rxflag == 1){

		if(IR_frame.irbuf[2] >= 0x80){		//ir�����ϱ�����Ϣ
		
			if(IR_frame.irbuf[IR_POSITION_COMMAND] == 0x42){		//ѧϰ����
							
				ir_learn_post_result();
							
			}
		
		}else{

			if(IR_frame.irbuf[2] == IR_frame.txbuf[2]){			//message id һ���������յ����͵Ļظ�

				if(IRTxSLHead && (IRTxSLHead->flag == 2)){		//���ڵȴ�����

					//��ʱ����ѧϰ�����10s�ȴ�����
					IRTxSLHead->flag = 3;			//���յ��ظ�

					if(IR_frame.irbuf[IR_POSITION_COMMAND] == 0xaa){		//��ִ

						if(IR_frame.irbuf[IR_POSITION_TYPE] == 0x01){		//�ɹ�ִ��
							
							ir_recepit_response(RECEIPT_CODE_SUCCESS, IRTxSLHead->msgid);	

						}else if(IR_frame.irbuf[IR_POSITION_TYPE] == 0x02){	//�ɹ�����
							
							ir_recepit_response(RECEIPT_CODE_ANALYZE_OK, IRTxSLHead->msgid);	
							
						}else if(IR_frame.irbuf[IR_POSITION_TYPE] == 0x03 || IR_frame.irbuf[IR_POSITION_TYPE] == 0x08){	//ʧ��
							
							ir_recepit_response(RECEIPT_CODE_FAILED, IRTxSLHead->msgid);	
							
						}else{

						}			
					}
					
				}
				else{			
				

				}

			}


		}

		IR_frame.rxflag = 0;
		
	}

}



/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void UART4_send(u8 *buf, u16 len)
{
	if(len >= IR_SEND_LEN){
		len = IR_SEND_LEN;
	}	
	memcpy(IR_frame.txbuf, buf, len);
	IR_frame.txlen = len;
	IR_frame.txhas = 0;
	
	USART_ITConfig(UART4, USART_IT_TXE, ENABLE);    //�����������������ж�
}

/*-------------------------------------------------------------------------
    ����
-------------------------------------------------------------------------*/
void UART4_recv(void)
{
	u8 temp;

	temp = (u8)(UART4->DR & (uint16_t)0x01FF);
	
	IR_frame.rxbuf[IR_frame.rxhas] = temp;
	IR_frame.rxhas++;

	switch(IR_frame.rxhas){
		
		case 1:
			if (temp != 0x7e) IR_frame.rxhas = 0;
			break;
			
		case 2:
			if (temp != 0x7e) IR_frame.rxhas = 0;
			break;
			
		case 3: break;
		case 4: break;
		case 5:
			IR_frame.rxlen = IR_frame.rxbuf[IR_POSITION_LENGTH] * 256 + IR_frame.rxbuf[IR_POSITION_LENGTH + 1];
			break;
		default:
			//���յ������Ժ����ж������Ƿ�������
			if(IR_frame.rxhas > IR_AFTER_LENGTH_BYTES){	

				if (IR_frame.rxhas >= IR_frame.rxlen + 3){	//�����������					
					//У����ȷ	
					if (XOR_Check(IR_frame.rxbuf + 2, IR_frame.rxlen) == IR_frame.rxbuf[IR_frame.rxhas - 1]){

						if(IR_frame.rxflag == 0){		//û�����ڴ�������
							IR_frame.rxflag = 1;
							memcpy(IR_frame.irbuf, IR_frame.rxbuf, IR_frame.rxhas);
							
						}						
					}					
					IR_frame.rxhas = 0;
					
				}

			}

			//��ֹ���մ�������
			if (IR_frame.rxhas >= IR_RECV_LEN){
				IR_frame.rxhas = 0;
			}

			break;
			
	}	

}


/*-----------------------------------------------------------------------
	UART4_IRQHandler �жϷ������ 
------------------------------------------------------------------------*/
void UART4_IRQHandler(void)                
{
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){		 //�����ж�

		UART4_recv();
		
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);        	 //����жϱ�־
		
	}

	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)		//�����ж�(���ͼĴ���Ϊ��ʱ)
	{
		if(IR_frame.txhas < IR_frame.txlen){
			
			UART4->DR = IR_frame.txbuf[IR_frame.txhas] & 0x01FF;
			IR_frame.txhas++;
			
		}else{
		
			IR_frame.txhas = 0;
			IR_frame.txlen = 0;
			
			if(IRTxSLHead->flag == 1){
				IRTxSLHead->flag = 2;	
			}

			USART_ITConfig(UART4, USART_IT_TXE, DISABLE);	//�رշ����ж�
			
		}
	}

	
} 



/*-------------------------------------------------------------------------
    100msִ��һ��
-------------------------------------------------------------------------*/
void IR_send(void)
{
//	if(IRTxSLHead){		//����ͷ��Ϊ��
//		UART4_send((u8 *)IRTxSLHead->data, IRTxSLHead->len);		//uart2TxSLHead->haswrite���ж��д���
//	}

	while(IRTxSLHead){
		
		if(IRTxSLHead->flag == 0){				//ͨѶ���̻�δ��ʼ
			
			UART4_send((u8 *)IRTxSLHead->data, IRTxSLHead->len);		
			IRTxSLHead->flag = 1;				//��ʼ����
			break;		
		}
		else if(IRTxSLHead->flag == 1 || IRTxSLHead->flag == 2){			//ͨѶ�����ѿ�ʼ

			IRTxSLHead->time_cnt++;
			if(IRTxSLHead->time_cnt > 10){			//1000ms�ȴ�ʱ�䵽ͨѶ���̻�û�н���
				deleteNodeFrom_IRTxSLHead();		
			}else{
				break;
			}
			
		}
//		else if(IRTxSLHead->flag == 1 || IRTxSLHead->flag == 2){			//ͨѶ�����ѿ�ʼ

//			
//		}
		else{								

			deleteNodeFrom_IRTxSLHead();			
		}

	}


}



























 
