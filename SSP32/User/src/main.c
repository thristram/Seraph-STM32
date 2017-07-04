#include "includes.h"
#include "sensors.h"

void Task2ms(void);
void Task100ms(void);
void Task300ms(void);
void Task1s(void);
void Task2s(void);

void system_init(void)
{
	//ss.meshid = 0x8005;
	//ss.sc[0].meshid = 0x8004;
	ss.sc[0].deviceid[0] = 0x53;
	ss.sc[0].deviceid[1] = 0x43;
	ss.sc[0].deviceid[2] = 0x35;
	ss.sc[0].deviceid[3] = 0x35;
	ss.sc[0].deviceid[4] = 0x41;
	ss.sc[0].deviceid[5] = 0x42;
	ss.sc[0].deviceid[6] = 0x35;
	ss.sc[0].deviceid[7] = 0x36;
	/*
	u8 i;
	for(i = 0; i < 20; i++) ss.st[i].meshid = 0x8001+i;
	Usart2_Send_Done = 0;
	for(i = 0;i < 5;i++)	{
		ss.sc[i].deviceid[0] = 'S';
		ss.sc[i].deviceid[1] = 'C';
		ss.sc[i].deviceid[2] = '1';
		ss.sc[i].deviceid[3] = '2';
		ss.sc[i].deviceid[4] = '3';
		ss.sc[i].deviceid[5] = '4';
		ss.sc[i].deviceid[6] = '5';
		ss.sc[i].deviceid[7] = '6';
	}
	*/
	broadcast_cnt = 25;//�����ϵ�5s����broadcast�㲥����id
}

 void test_gpio_config(void)
 {
	 
	 GPIO_InitTypeDef GPIO_InitStructure;
	 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //????
 
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 /* PA4 */									 
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //??????50MHz
 //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			 //???????
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
 
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
 
 //  GPIO_SetBits(GPIOA, GPIO_Pin_4);
	 GPIO_ResetBits(GPIOA, GPIO_Pin_4);
 }
 
 
 void test_gpio_pwm(void)
 {

	static u8 flag = 0;
	 if(flag)	{GPIO_SetBits(GPIOA, GPIO_Pin_4);flag = 0;}
	 else			{GPIO_ResetBits(GPIOA, GPIO_Pin_4);flag = 1;}
	





 
 }


 int main(void)
 {
	u8 *hello = "Hello!";
	TIM2_Init();
	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_Configuration();// �����ж����ȼ�����
	uart1_init(115200);	 //���ڳ�ʼ��Ϊ115200����4004ͨѶ
	uart2_init(57600);    //���ڲ�����Ϊ57600����1010ͨѶ
	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ� 
	system_init();
	//printf("Hello!\r\n");
	Usart2_Send(hello,6);
	while(!Usart2_Send_Done);Usart2_Send_Done = 0;
	 
	sensors_init_all();
	test_gpio_config();
	 
	//test_cjson();
	while(1)
	{
		Task2ms();
		Task100ms();
		Task300ms();
		Task1s();
		Task2s();
	}	 
}


void Task2ms(void)
{
	if(f_2ms)
	{
		f_2ms = 0;
		test_gpio_pwm();
		PYD1798_check();
		smokeModule_check();
		analyze();//���յ�4004��Ϣ����
		rev_anaylze();//���յ�1010��Ϣ����
		/*if(rev_success2)
		{
			rev_success2 = 0;
			sicp_action_cmd();
		}*/
	}
}

void Task100ms(void)
{
	if(f_100ms)
	{
		f_100ms = 0;
		//init_send_Txmessage(CONTINUE);
		/*******SICP************/
		if(ack_ar)	{ack_ar = 0;sicp_cmd_refresh();success_receipt();}
		sicp_cmd_data();
		sicp_led_cmd();
		sicp_alert_cmd();
		sicp_qe_action_cmd();
		if(ack_dr)	{ack_dr = 0;send_dr_refresh_cmd_done =1;sicp_cmd_refresh();}//send_data_sync(0x83)���յ�ST�ظ����ٷ���}
		/*******SSP************/
		rev_action_perform();
		rev_qe();
		rev_device_status();
		send_device_info_sub();
		if(rev_20 && send_dr_refresh_cmd_done){rev_20 = 0;send_dr_refresh_cmd_done = 0;send_data_sync(0x83);}
		
		CO2_usart3_send();
	}
}

void Task300ms(void)
{
	if(f_300ms)
	{
		f_300ms = 0;
		ADC1_value_filter();
	}
}

void Task1s(void)
{
	
	if(f_1s)
	{
		f_1s = 0;
		//success_receipt();
		LED1_Toggle;
		LED2_Toggle;
		LED3_Toggle;
		LED4_Toggle;
		sicp_send_heartbeart();
		sicp_broadcast();
		if(sicp_refr > 0){
			sicp_refr--;
			if(sicp_refr == 0){
				sicp_cmd_refresh();
			}
		}
	}
}

void Task2s(void)
{
	if(f_2s)
	{
		f_2s = 0;
		SHT3X_read();
	}
}
