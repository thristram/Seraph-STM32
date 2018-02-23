
#include "includes.h"
#include "sensors.h"
#include "key_string.h"
#include "ir.h"



u32 tv_sec = 0;//记录系统上电运行时间

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void system_init(void)
{
	
	memset(&ss, 0, sizeof(ss));
	
//	ss.deviceid[0] = 'S';
//	ss.deviceid[1] = 'S';
//	ss.deviceid[2] = 'E';
//	ss.deviceid[3] = '1';
//	ss.deviceid[4] = '1';
//	ss.deviceid[5] = 'T';
//	ss.deviceid[6] = '2';
//	ss.deviceid[7] = '6';
//	ss.deviceid[8] = 0; 
//	ss.deviceid[9] = 0;
//	ss.deviceid[10] = 0;
//	ss.deviceid[11] = 0;
	

ss.deviceid[0] = 'S';
ss.deviceid[1] = 'S';
ss.deviceid[2] = '8';
ss.deviceid[3] = '8';
ss.deviceid[4] = '8';
ss.deviceid[5] = '8';
ss.deviceid[6] = '8';
ss.deviceid[7] = '8';
ss.deviceid[8] = '8'; 
ss.deviceid[9] = '8';
ss.deviceid[10] = 0;
ss.deviceid[11] = 0;



	//rt
	memset(ss_rt.sepid,0,12);
	memset(ss_rt.sepid2,0,12);

	//action backlight
	memset(ss_ab.sepid,0,12);

	Usart1_Delay_Cnt = 0;
			
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
	 else		{GPIO_ResetBits(GPIOA, GPIO_Pin_4);flag = 1;}

 }
	

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void Task2ms(void)
{
	if(f_2ms)
	{
		f_2ms = 0;
		//test_gpio_pwm();
		PYD1798_check();
		smokeModule_check();
		

	}
}

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void Task100ms(void)
{
	if(f_100ms)
	{
		f_100ms = 0;

		if(cmd_led_flag){		//有数据接收,闪烁LED
			system_led_show();
		}

		/* 循环广播 */
		sicp_broadcast_loop();



		if(uart2TxSLHead){		//链表头不为空
			USART2_send((u8 *)uart2TxSLHead->data, uart2TxSLHead->len); 		//uart2TxSLHead->haswrite在中断中处理
		}

		/*----------------- IR -----------------*/

		IR_send();


		/*----------------- SICP -----------------*/
		if(ack_ar){
			ack_ar = 0;
			sicp_cmd_refresh();
			ssp_recepit_response(RECEIPT_CODE_SUCCESS);
		}
		
		sicp_cmd_data();
		sicp_led_cmd();
		
		reSendList_send();


		if(ack_dr){
			ack_dr = 0; 
			SICP_handle.status.bit.sensorsFreshCmdSended = 1;
			sicp_cmd_refresh();

		}


		/*----------------- SSP -----------------*/
		
		if(SICP_handle.status.bit.sensorsFreshCmdSended){
			SICP_handle.status.bit.sensorsFreshCmdSended = 0; 
			ssp_data_sync(SSP_POST);
		}		
//		send_rt();

		ssp_send_data_active();

		/*----------------- CO2 -----------------*/
		
		CO2_usart3_send();
		
		LED_test();
		
	}
	
}

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void Task300ms(void)
{
	if(f_300ms)
	{
		f_300ms = 0;
		
		ADC1_value_filter();
	
	}
}

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void Task1s(void)
{
	if(f_1s)
	{
		f_1s = 0;
		
		tv_sec++;
		
		if(cmd_led_flag == 0){	//没有数据接收，正常翻转LED
			SYSTEM_LED_REVERSE;
		}

		
		sicp_send_heartbeat();

		
		if(sicp_refr > 0){
			sicp_refr--;
			if(sicp_refr == 0){
				sicp_cmd_refresh();
			}
		}

		/* 检查设备的合法性 */
//		devList.times++;
//		if(devList.times >= DEVICE_LEGALITY_CHECK_TIME){
//			devList.times = 0;
//			ssp_check_device_legality();
//		}

		/* 运动检测 */
		ssp_motion_detect_post();
		
		/* 故障检测 */
		ssp_device_malfunction_detect();


//		IR_test();
		
	}
	
}

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void Task2s(void)
{
	if(f_2s){
		
		f_2s = 0;
		
		SHT3X_read();

		sicp_config_st_send();

		
	}
}


/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
void test000(void)
{
	int size = 0;

	size = sizeof(Txmessage);
	size = sizeof(Rxmessage);
	size = sizeof(SS);
	size = sizeof(AB);
	size = sizeof(SS_sense);
	size = sizeof(CS);
	size = sizeof(AR);
	size = sizeof(RT);
	size = sizeof(SL_AP);
	size = sizeof(SP_AP);

	
	size = sizeof(CSHP);

	size = sizeof(CSHP);



}


/*-------------------------------------------------------------------------
    1/72M=0.01389us
-------------------------------------------------------------------------*/
 int main(void)
 {
	

	TIM2_Init();			/* 时间管理 */
	delay_init();	    	 	//延时函数初始化	
	NVIC_Configuration();	// 设置中断优先级分组
	USART1_init(115200);	/* 串口初始化为115200，与4004通讯 */
	USART2_init(57600);    	/* 串口波特率为57600，与1010通讯 */
	IR_Module_init();

//	IR_init();

	system_led_Init();		  	//初始化与LED连接的硬件接口 
	system_init();
	
	LED_Init();

	 
	sensors_init_all();
	test_gpio_config();
	
	test000(); 
	
	while(1)
	{

		recv_4004_analyze();		//接收到4004消息处理
		sicp_recv_analyze();		//接收到1010消息处理


		IR_recv_handle();
				
		
		Task2ms();
		Task100ms();
		
		Task300ms();
		Task1s();
		Task2s();
		
		
//		SYSTEM_LED_REVERSE;
		
	}	 
	
}














