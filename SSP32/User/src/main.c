#include "includes.h"
#include "sensors.h"

u16 sys_time;//记录系统上电运行时间
u8 tick400ms;

void Task2ms(void);
void Task100ms(void);
void Task300ms(void);
void Task1s(void);
void Task2s(void);

void system_init(void)
{
	u8 i,j,k;
	ss.deviceid[0] = 'S';
	ss.deviceid[1] = 'S';
	ss.deviceid[2] = 'E';
	ss.deviceid[3] = '1';
	ss.deviceid[4] = '1';
	ss.deviceid[5] = 'T';
	ss.deviceid[6] = '2';
	ss.deviceid[7] = '6';
	for(i = 0; i < 5; i++)
	{
		for(j = 0;j < 8;j++)
		{
			ss.sc[i].deviceid[j] = 0x00;
			ss.sc[i].posted = 0;
		}
	}
	for(i = 0; i < 20; i++)
	{
		for(j = 0;j < 8;j++)
		{
			ss.st[i].deviceid[j] = 0x00;
			ss.st[i].posted = 0;
		}
	}
	for(i = 0; i < 5; i++)
	{
		for(j = 0;j < 15;j++)
		{
			ss.sc[i].slc[j].posted = 0;
			ss.sc[i].spc[j].posted = 0;
			for(k= 0; k < 8; k++)
			{
				ss.sc[i].slc[j].deviceid[k] = 0x00;
				ss.sc[i].spc[j].deviceid[k] = 0x00;
			}
		}
	}
	ss.sc[0].meshid = 0x8010;
	//ss.st[0].deviceid[0] = 'A';
	//ss.st[0].deviceid[1] = 'A';
	//ss.st[0].deviceid[2] = '5';
	//ss.st[0].deviceid[3] = '5';
	//ss.st[0].deviceid[4] = 'A';
	//ss.st[0].deviceid[5] = 'B';
	//ss.st[0].deviceid[6] = '7';
	//ss.st[0].deviceid[7] = '0';
	//ss.meshid = 0x8005;
	//ss.sc[0].meshid = 0x8004;
	/*ss.sc[0].deviceid[0] = 0x41;
	ss.sc[0].deviceid[1] = 0x41;
	ss.sc[0].deviceid[2] = 0x35;
	ss.sc[0].deviceid[3] = 0x35;
	ss.sc[0].deviceid[4] = 0x41;
	ss.sc[0].deviceid[5] = 0x42;
	ss.sc[0].deviceid[6] = 0x35;
	ss.sc[0].deviceid[7] = 0x36;
	ss.st[0].deviceid[0] = 0x53;
	ss.st[0].deviceid[1] = 0x54;
	ss.st[0].deviceid[2] = 0x35;
	ss.st[0].deviceid[3] = 0x35;
	ss.st[0].deviceid[4] = 0x41;
	ss.st[0].deviceid[5] = 0x42;
	ss.st[0].deviceid[6] = 0x35;
	ss.st[0].deviceid[7] = 0x39;
	ss.sc[0].slc[0].deviceid[0] = 0x41;
	ss.sc[0].slc[0].deviceid[1] = 0x41;
	ss.sc[0].slc[0].deviceid[2] = 0x35;
	ss.sc[0].slc[0].deviceid[3] = 0x35;
	ss.sc[0].slc[0].deviceid[4] = 0x41;
	ss.sc[0].slc[0].deviceid[5] = 0x42;
	ss.sc[0].slc[0].deviceid[6] = 0x35;
	ss.sc[0].slc[0].deviceid[7] = 0x37;
	ss.sc[0].slc[2].deviceid[0] = 0x41;
	ss.sc[0].slc[2].deviceid[1] = 0x41;
	ss.sc[0].slc[2].deviceid[2] = 0x35;
	ss.sc[0].slc[2].deviceid[3] = 0x35;
	ss.sc[0].slc[2].deviceid[4] = 0x41;
	ss.sc[0].slc[2].deviceid[5] = 0x42;
	ss.sc[0].slc[2].deviceid[6] = 0x35;
	ss.sc[0].slc[2].deviceid[7] = 0x39;
	ss.sc[0].spc[1].deviceid[0] = 0x41;
	ss.sc[0].spc[1].deviceid[1] = 0x41;
	ss.sc[0].spc[1].deviceid[2] = 0x35;
	ss.sc[0].spc[1].deviceid[3] = 0x35;
	ss.sc[0].spc[1].deviceid[4] = 0x41;
	ss.sc[0].spc[1].deviceid[5] = 0x42;
	ss.sc[0].spc[1].deviceid[6] = 0x35;
	ss.sc[0].spc[1].deviceid[7] = 0x38;*/
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
	broadcast_cnt = 25;//初次上电5s后发送broadcast广播网关id
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
	delay_init();	    	 //延时函数初始化	
	NVIC_Configuration();// 设置中断优先级分组
	uart1_init(115200);	 //串口初始化为115200，与4004通讯
	uart2_init(57600);    //串口波特率为57600，与1010通讯
	LED_Init();		  	 //初始化与LED连接的硬件接口 
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
		analyze();//接收到4004消息处理
		rev_anaylze();//接收到1010消息处理
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
		if(tick400ms < 3)	tick400ms++;
		else{
			tick400ms = 0;
			if(uart2TxSLHead){//链表头不为空
				Usart2_Send(uart2TxSLHead->data,uart2TxSLHead->len); //uartTxSLHead->haswrite在中断中处理
			}
		}
		
		//init_send_Txmessage(CONTINUE);
		/*******SICP************/
		if(ack_ar)	{ack_ar = 0;sicp_cmd_refresh();success_receipt();}
		sicp_cmd_data();
		sicp_led_cmd();
		sicp_alert_cmd();
		sicp_qe_action_cmd();
		if(ack_dr)	{ack_dr = 0;send_dr_refresh_cmd_done =1;sicp_cmd_refresh();}//send_data_sync(0x83)在收到ST回复后再发送}
		/*******SSP************/
		rev_action_perform();
		rev_qe();
		rev_device_status();
		if(ack_dl)	send_device_info_sub(0x01,0x00,0x00);//要等待esh发送device list得到sub设备的device id
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
		if(sys_time < 65000)	sys_time++;
		else									sys_time=65000;
		if(sys_time == 3)			send_device_info_ss(0x01,0x00,0x00);//上电3s后主动推送1次ss的device info
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
