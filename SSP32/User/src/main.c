#include "includes.h"


void Task2ms(void);
void Task100ms(void);
void Task300ms(void);
void Task1s(void);
void Task2s(void);

void system_init(void)
{
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
}

 int main(void)
 {
	u8 *hello = "Hello!";
	TIM2_Init();
	delay_init();	    	 //延时函数初始化	
	NVIC_Configuration();// 设置中断优先级分组
	uart1_init(115200);	 //串口初始化为9600
	uart2_init(57600);
	LED_Init();		  	 //初始化与LED连接的硬件接口 
	system_init();
	//printf("Hello!\r\n");
	Usart2_Send(hello,6);
	 while(!Usart2_Send_Done);Usart2_Send_Done = 0;
	//test_cjson();

	while(1)
	{
		Task2ms();
		Task100ms();
		Task1s();
	}	 
}


void Task2ms(void)
{
	if(f_2ms)
	{
		f_2ms = 0;
		analyze();
		rev_anaylze();
		if(rev_success2)
		{
			rev_success2 = 0;
			//init_send_Txmessage(CONFIG_SS);
			//send_config_ss();
			sicp_action_cmd();
		}
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
		sicp_led_cmd();
		sicp_alert_cmd();
		if(ack_dr)	{ack_dr = 0;send_dr_refresh_cmd_done =1;sicp_cmd_refresh();}//send_data_sync(0x83)在收到ST回复后再发送}
		/*******SSP************/
		rev_action_perform();
		rev_qe();
		rev_device_status();
		send_device_info_sub();
		if(rev_20 && send_dr_refresh_cmd_done){rev_20 = 0;send_dr_refresh_cmd_done = 0;send_data_sync(0x83);}
	}
}

void Task300ms(void)
{
	if(f_300ms)
	{
		f_300ms = 0;
	}
}

void Task1s(void)
{
	
	if(f_1s)
	{
		f_1s = 0;
		LED1_Toggle;
		LED2_Toggle;
		LED3_Toggle;
		LED4_Toggle;
		sicp_send_heartbeart();
		//sicp_receipt(0x01,0x33,0x8000);
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
	}
}
