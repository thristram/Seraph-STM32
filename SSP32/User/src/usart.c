
#define _USART_GLOBAL
#include "includes.h"
#include "stdio.h"
#include "stdlib.h"




USART2_handle_t USART2_handle = {0};



#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;

};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
_sys_exit(int x)
{
	x = x;
}
//�ض���fputc����

int fputc(int ch, FILE *f)
{
	while((USART2->SR & 0X40)==0);//ѭ������,ֱ���������
	USART2->DR = (u8)ch;
	return ch;
}

#endif


/*-------------------------------------------------------------------------
//bound:������    
-------------------------------------------------------------------------*/
void USART1_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

	USART_DeInit(USART1);  //��λ����1
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//���������ж�
	USART_ClearFlag(USART1,USART_IT_TC);					//���������ɱ�־
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);   //ʹ�ܴ���1 DMA����
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���

	//��Ӧ��DMA����
	DMA_DeInit(DMA1_Channel5);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����1��Ӧ����DMAͨ��5
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;  //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)DMA_Rece_Buf;  	//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  	//���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_InitStructure.DMA_BufferSize = DMA_BUF_LEN;  		//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  		//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  		//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);  		//����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���

	DMA_Cmd(DMA1_Channel5, ENABLE);  //��ʽ����DMA����

}

/*-------------------------------------------------------------------------    
	//���»ָ�DMAָ��
-------------------------------------------------------------------------*/
void MYDMA_Enable(DMA_Channel_TypeDef * DMA_CHx)
{
	DMA_Cmd(DMA_CHx, DISABLE );  	//�ر�USART1 TX DMA1 ��ָʾ��ͨ��
	DMA_SetCurrDataCounter(DMA_CHx, DMA_BUF_LEN);		//DMAͨ����DMA����Ĵ�С
	DMA_Cmd(DMA_CHx, ENABLE);  	//ʹ��USART1 TX DMA1 ��ָʾ��ͨ��
}

//�ַ�������
u8 * copystring(u8 *des,u8 *src)
{
	u8 *tmp=des;
	while( *tmp++ = *src++ ) ;
	return des;
}


//����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���
/*
void Usart1_Send(u8 *buf,u8 len)
{
	u8 t;
  	for(t=0;t<len;t++)		//ѭ����������
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(USART1,buf[t]);
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}
*/
void Usart1_Send(u8 *buf,u16 len)
{
	if(len >= USART1_TXBUF_LEN)	len = USART1_TXBUF_LEN;
	mymemcpy(Usart1_Send_Buf, buf,len);
	Usart1_Send_Length = len;
	Usart1_Send_Cnt = 0;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);    //�����������������ж�
}




/*-------------------------------------------------------------------------
//�ӽ��ջ�����ȡ����BB BB��ͷ����0A 0A��β�����ݰ�
-------------------------------------------------------------------------*/
u16 get_sspbuf(u16 src_len)
{
	u16 i, ret = 0;

	if((DMA_Rece_Buf[0] == 0xAA) && (DMA_Rece_Buf[1] == 0xAA))
	{

		ss.flag.bit.tcp_en = (DMA_Rece_Buf[6] == 0x01) ? 1 : 0;
	}

	if((DMA_Rece_Buf[0] == 0xBB) && (DMA_Rece_Buf[1] == 0xBB))
	{

		if((DMA_Rece_Buf[src_len -2] == 0x0A) && (DMA_Rece_Buf[src_len -1] == 0x0A))
		{

			ret = src_len - 2;
			for(i = 0; i < ret; i++)
			{
				ssp_buf[i] = DMA_Rece_Buf[i];
			}
			ssp_buf[ret] = 0;
		}
	}

	return ret;

}


/*-------------------------------------------------------------------------
//�����жϺ���
-------------------------------------------------------------------------*/
void USART1_IRQHandler(void)                	//����1�жϷ������
{

	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET){

		USART_ReceiveData(USART1);					//��ȡ���� ע�⣺������Ҫ�������ܹ�����жϱ�־λ����Ҳ��֪��Ϊɶ��

		Usart1_Rec_Cnt = DMA_BUF_LEN -DMA_GetCurrDataCounter(DMA1_Channel5);		//����ӱ�֡���ݳ���

		ssp_length = get_sspbuf(Usart1_Rec_Cnt);
		rev_success = 1;
		Usart1_Delay_Cnt = 0;

		USART_ClearITPendingBit(USART1, USART_IT_IDLE);         //����жϱ�־
		MYDMA_Enable(DMA1_Channel5);                   			//�ָ�DMAָ�룬�ȴ���һ�εĽ���
	}

	/* //�����ж�(���ͼĴ���Ϊ��ʱ) */
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET){

		if(Usart1_Send_Cnt < Usart1_Send_Length)
		{

			USART1->DR = Usart1_Send_Buf[Usart1_Send_Cnt]&0x01FF;
			Usart1_Send_Cnt++;
		}
		else
		{

			Usart1_Send_Cnt = 0;
			if(!ssp_uart_mutex)	deleteNodeFromUartTxSLHead();//��������uartRxSLHead��uartRxSLLast��addNodeToUartRxSLLast������δ������ͱ���
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	//�رշ����ж�
		}
	}

}



/*-------------------------------------------------------------------------
    //bound:������
-------------------------------------------------------------------------*/
void USART2_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//ʹ��USART2ʱ��

	USART_DeInit(USART2);  //��λ����1

	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9

	//USART2_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;		//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ClearFlag(USART2,USART_IT_TC);					//���������ɱ�־
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���

}


/*-------------------------------------------------------------------------
//buf:�������׵�ַ
//len:���͵��ֽ���
-------------------------------------------------------------------------*/
void USART2_send(u8 *buf, u16 len)
{
	if(len >= USART2_TX_BUF_LEN) len = USART2_TX_BUF_LEN;
	
	mymemcpy(USART2_handle.txBuf, buf, len);
	
	USART2_handle.txLen = len;
	USART2_handle.txCnt = 0;
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);    //�����������������ж�
}


/*----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/
void USART2_recv(void)
{
	u8 temp;
	u8 check_sum;
	
	temp = (u8)USART_ReceiveData(USART2);
	
	USART2_handle.rxBuf[USART2_handle.rxCnt] = temp;
	USART2_handle.rxCnt++;
	switch(USART2_handle.rxCnt)
	{
		case 1:
			if ((temp != 0xEE) && (temp != 0xDD))	USART2_handle.rxCnt = 0;
			break;
		case 2:
			if ((temp != 0xEE) && (temp != 0xDD) && (temp != 0xAA)) USART2_handle.rxCnt = 0;
			break;
		case 3:
			//if (!temp)				rev_count = 0;
			break;
		case 4:
			//if (!temp)				rev_count = 0;
			break;
		case 5:
			//if (!temp)				rev_count = 0;
			break;
		default:
			if (USART2_handle.rxCnt >= USART2_RX_BUF_LEN){		//��ֹ���մ�������
				USART2_handle.rxCnt = 0;
			}			
			if ((USART2_handle.rxBuf[0] == 0xEE) && ((USART2_handle.rxBuf[1] == 0xEE) || (USART2_handle.rxBuf[1] == 0xAA))){

				if (USART2_handle.rxCnt > USART2_handle.rxBuf[5] + 2){		//�����������
				
					check_sum = Check_Sum(USART2_handle.rxBuf + 2, USART2_handle.rxBuf[5]);					
					if (check_sum == USART2_handle.rxBuf[USART2_handle.rxBuf[5] + 2]){		//У����ȷ
						SICP_handle.status.bit.recvFlag = 1;
						memcpy(SICP_handle.rxBuf, USART2_handle.rxBuf, USART2_handle.rxCnt);
					}
					USART2_handle.rxCnt = 0;

				}
			}
			else if((USART2_handle.rxBuf[0] == 0xDD) && (USART2_handle.rxBuf[1] == 0xDD)){
				
				if (USART2_handle.rxCnt > USART2_handle.rxBuf[3] + 2){		//�����������		
					SICP_handle.status.bit.recvFlag = 1;
					memcpy(SICP_handle.rxBuf, USART2_handle.rxBuf, USART2_handle.rxCnt);
					USART2_handle.rxCnt = 0;
				}
			}
			else{
				
				USART2_handle.rxCnt = 0;
			}
			break;
			
	}
	
}


/*-------------------------------------------------------------------------
//�����жϺ���
-------------------------------------------------------------------------*/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){

		USART2_recv();
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}

	/* //�����ж�(���ͼĴ���Ϊ��ʱ) */
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET){

		if(USART2_handle.txCnt < USART2_handle.txLen){
			USART2->DR = USART2_handle.txBuf[USART2_handle.txCnt] & 0x01FF;
			USART2_handle.txCnt++;

		}else{
			USART2_handle.txCnt = 0;
			if(!sicp_uart_mutex)	deleteNodeFromUart2TxSLHead();		//��������uartRxSLHead��uartRxSLLast��addNodeToUartRxSLLast������δ������ͱ���
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);			//�رշ����ж�

		}
	}

}

























