#include "includes.h"
#include "sensors.h"

u16 sys_time;//��¼ϵͳ�ϵ�����ʱ��
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
	ss.deviceid[8] = 0; 
	ss.deviceid[9] = 0;
	ss.deviceid[10] = 0;
	ss.deviceid[11] = 0;
	for(i = 0; i < 5; i++)
	{
		for(j = 0;j < 12;j++)
		{
			ss.sc[i].deviceid[j] = 0x00;
			ss.sc[i].posted = 0;
		}
	}
	for(i = 0; i < 20; i++)
	{
		for(j = 0;j < 12;j++)
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
			for(k= 0; k < 12; k++)
			{
				ss.sc[i].slc[j].deviceid[k] = 0x00;
				ss.sc[i].spc[j].deviceid[k] = 0x00;
			}
		}
	}
	ss.sc[0].deviceid[0] = 'S';
	ss.sc[0].deviceid[1] = 'C';
	ss.sc[0].deviceid[2] = '0';
	ss.sc[0].deviceid[3] = '9';
	ss.sc[0].deviceid[4] = '8';
	ss.sc[0].deviceid[5] = 'A';
	ss.sc[0].deviceid[6] = '7';
	ss.sc[0].deviceid[7] = '6';
	ss.sc[0].deviceid[8] = '1';
	ss.sc[0].deviceid[9] = '2';
	ss.sc[0].meshid = 0x8010;   //for test
	ss.sc[0].spc[0].MDID = 1;   //for test
	ss.sc[0].spc[1].MDID = 2;   //for test
	//confit st�ṹ���ʼ��
	for(i = 0; i < 20; i++)	memset(ss_cst[i].target_id,0,12);
	//device status
	memset(ss_des.sepid,0,12);memset(ss_des.ch,0,2);
	//rt
	memset(ss_rt.sepid,0,12);
	memset(ss_rt.sepid2,0,12);
	//action perform
	for(i = 0; i < 10;i++)	{memset(ss_ap.sl_ap[i].sepid,0,12);memset(ss_ap.sp_ap[i].sepid,0,12);memset(action[10].deviceid,0,12);}
	//action backlight
	memset(ss_ab.sepid,0,12);
	//action config ss
	memset(ss.config.ssid,0,100);memset(ss.config.paswd,0,50);memset(ss.config.city,0,20);
	memset(ss.config.country,0,20);memset(ss.config.region,0,20); memset(ss.config.hm,0,10);
	memset(ss.config.pr,0,10);memset(ss.config.visibility,0,10);memset(ss.config.sunrise,0,6);
	memset(ss.config.sunset,0,6);memset(ss.config.code,0,10);memset(ss.config.tp,0,10);memset(ss.config.index,0,10);
  //ir
	memset(ss.ir.code,0,20);memset(ss.ir.address,0,10);memset(ss.ir.other,0,10);memset(ss.ir.raw,0,800);				
  //device status
	memset(ss_des.sepid,0,12);
	//qe
	memset(ss_qe.sepid,0,12);memset(ss_qe.action,0,4);memset(ss_qe.code,0,10);memset(ss_qe.raw,0,500);
	//alarm
	memset(ss_alarm.sepid,0,12);
		
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
	
 /////////////////////////////////////
			//ˢ�´���������//
 /////////////////////////////////////
 void ss_sensor_refresh(void)
 {
	 ss.sense.temperature = (int)(sensors_value.sht3x.temperature*10);
	 ss.sense.humidity 		= (u8)sensors_value.sht3x.humidity;
	 /*
	cJSON_AddNumberToObject(sub_cs,"HM",(int)ss.sense.humidity);sensors_value.sht3x.humidity
	cJSON_AddNumberToObject(sub_cs,"TP",(int)ss.sense.temperature);
	cJSON_AddNumberToObject(sub_cs,"PT",((int)(ss.sense.pm2_5_H<<8) + (int)ss.sense.pm2_5_L));
	cJSON_AddNumberToObject(sub_cs,"SM",(int)ss.sense.smoke);
	cJSON_AddNumberToObject(sub_cs,"PR",(int)ss.sense.human_sensing);
	cJSON_AddNumberToObject(sub_cs,"MI",(int)ss.sense.motion);
	cJSON_AddNumberToObject(sub_cs,"BT",(int)ss.sense.bt);
	cJSON_AddNumberToObject(sub_cs,"CO",((int)(ss.sense.CO_H<<8) + (int)ss.sense.CO_L));
	cJSON_AddNumberToObject(sub_cs,"CD",((int)(ss.sense.CO2_H<<8) + (int)ss.sense.CO2_L));
	cJSON_AddNumberToObject(sub_cs,"VO",(int)ss.sense.VOC);*/
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
		//test_gpio_pwm();
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
		if(tick400ms < 3)	tick400ms++;
		else{
			tick400ms = 0;
			if(uart2TxSLHead){//����ͷ��Ϊ��
				Usart2_Send(uart2TxSLHead->data,uart2TxSLHead->len); //uartTxSLHead->haswrite���ж��д���
			}
		}
		
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
		//rev_device_status();
		if(ack_dl)	send_device_info_sub(0x01,0x00,0x00);//Ҫ�ȴ�esh����device list�õ�sub�豸��device id
		if(rev_20 && send_dr_refresh_cmd_done){rev_20 = 0;send_dr_refresh_cmd_done = 0;send_data_sync(0x83);}
		send_rt();
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
		if(sys_time == 3)			send_device_info_ss(0x01,0x00,0x00);//�ϵ�3s����������1��ss��device info
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
		if(sensors_value.removeFlag){
			sensors_printf("\n remove");
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
