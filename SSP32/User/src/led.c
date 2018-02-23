#define _LED_GLOBAL
#include "includes.h"

u8	cmd_led_flag = 0;



/*-------------------------------------------------------------------------
    系统LED运行指示灯
-------------------------------------------------------------------------*/
void system_led_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PD端口时钟

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	    		 //LED1-->PD.2 端口配置, 推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	SYSTEM_LED_OFF;

 
}



/*----------------------------------------------------------------------------
	系统LED 控制
-----------------------------------------------------------------------------*/
void system_led_show(void)
{
	static u8 cnt = 0;

	if(cmd_led_flag == 1){		//接收到控制
		
		SYSTEM_LED_REVERSE;
		
		cnt++;
		if(cnt > 5){			
			cnt = 0;
			cmd_led_flag = 0;			
		}
	}
	
}


/*-------------------------------------------------------------------------
    灯带LED初始化
-------------------------------------------------------------------------*/
void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(LED1_RCC_CLOCK, ENABLE);	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;	    		 
	GPIO_Init(LED1_GPIO_BASE, &GPIO_InitStructure);	  				
	LED1_OFF; 						

	RCC_APB2PeriphClockCmd(LED2_RCC_CLOCK, ENABLE); 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;				 
	GPIO_Init(LED2_GPIO_BASE, &GPIO_InitStructure); 				
	LED2_OFF;						



}


/*-----------------------------------------------------------------------
	>50us
------------------------------------------------------------------------*/
void LED_RESET_hold(u16 us) 
{
	u16 i;
	
	for(i = 0; i < us; i++){
		
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();

		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();

		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();

		__nop(); __nop(); __nop(); __nop(); __nop();
//		__nop(); __nop(); __nop(); __nop(); __nop();

	}
		
}


/*-----------------------------------------------------------------------
	0.3us
------------------------------------------------------------------------*/
void LED_SW2812_T0H_hold(void) 
{	
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
//		__nop(); __nop(); __nop(); __nop(); __nop();
		
//		__nop(); __nop(); __nop(); __nop(); __nop();
//		__nop(); __nop(); __nop(); __nop(); __nop();
		

}

/*-----------------------------------------------------------------------
	0.9us
------------------------------------------------------------------------*/
void LED_SW2812_T0L_hold(void) 
{	
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();		
		__nop(); __nop(); 
		
//		__nop(); __nop(); __nop(); __nop(); __nop();

//		__nop(); __nop(); __nop(); __nop(); __nop();

}

/*-----------------------------------------------------------------------
	0.6us
------------------------------------------------------------------------*/
void LED_SW2812_T1H_hold(void) 
{
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();

		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); 
}

/*-----------------------------------------------------------------------
	0.6us
------------------------------------------------------------------------*/
void LED_SW2812_T1L_hold(void) 
{
		__nop(); __nop(); __nop(); __nop(); __nop();
//		__nop(); __nop(); __nop(); __nop(); __nop();
		
//		__nop(); __nop(); __nop(); __nop(); __nop();		

}



/*-----------------------------------------------------------------------
	使用结构体的方式虽然对程序上有帮助，
	但是在发送时每8bit会有一多余的延迟
------------------------------------------------------------------------*/
void LED_SW2812_show0(LED_show_t *pdata, u16 num) 
{
	u16 count;
	u8 i;

	for (count = 0; count < num; count++){

		//按照GRB的顺序
		//green
		for (i = 0; i < 8; i++){					
			if(pdata[count].green & BIT[7 -i]){		// 1			
				LED1_ON;	LED_SW2812_T1H_hold();				
				LED1_OFF;	LED_SW2812_T1L_hold();				
			}else{			
				LED1_ON;	LED_SW2812_T0H_hold();				
				LED1_OFF;	LED_SW2812_T0L_hold();
			}						
		}
		
		//reg
		for (i = 0; i < 8; i++){					
			if(pdata[count].reg & BIT[7 -i]){		// 1			
				LED1_ON;	LED_SW2812_T1H_hold();				
				LED1_OFF;	LED_SW2812_T1L_hold();				
			}else{			
				LED1_ON;	LED_SW2812_T0H_hold();				
				LED1_OFF;	LED_SW2812_T0L_hold();
			}						
		}

		//blue
		for (i = 0; i < 8; i++){					
			if(pdata[count].blue & BIT[7 -i]){ 		// 1			
				LED1_ON;	LED_SW2812_T1H_hold(); 			
				LED1_OFF;	LED_SW2812_T1L_hold(); 			
			}else{			
				LED1_ON;	LED_SW2812_T0H_hold(); 			
				LED1_OFF;	LED_SW2812_T0L_hold();
			}						
		}
		
	}

}


/*-----------------------------------------------------------------------
	0xggrrbb00
------------------------------------------------------------------------*/
void LED_SW2812_show(u32 *pdata, u16 num) 
{
	u16 i;
	u32 temp;

	for (i = 0; i < num; i++){

		//按照GRB的顺序

		temp = *(pdata + i);		
		for (i = 0; i < 24; i++){							
			if(temp >= 0x80000000 ){		// 1			
				LED1_ON;	LED_SW2812_T1H_hold();				
				LED1_OFF;	LED_SW2812_T1L_hold();				
			}else{			
				LED1_ON;	LED_SW2812_T0H_hold();				
				LED1_OFF;	LED_SW2812_T0L_hold();
			}
			temp <<= 1;
		}		
	}

}

/*-----------------------------------------------------------------------
	0.4us
------------------------------------------------------------------------*/
void LED_SW2812B_T0H_hold(void) 
{	
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); 
//		__nop(); __nop(); __nop(); __nop(); __nop();
//		__nop(); __nop(); __nop(); __nop(); __nop();
		

}

/*-----------------------------------------------------------------------
	0.8us
------------------------------------------------------------------------*/
void LED_SW2812B_T0L_hold(void) 
{	
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop();	
		
//		__nop(); __nop(); __nop(); __nop(); __nop();

//		__nop(); __nop(); __nop(); __nop(); __nop();

}

/*-----------------------------------------------------------------------
	0.8us
------------------------------------------------------------------------*/
void LED_SW2812B_T1H_hold(void) 
{
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();

		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
		__nop(); __nop(); __nop(); __nop(); __nop();
//		__nop(); __nop(); __nop();
		
}

/*-----------------------------------------------------------------------
	0.4us
------------------------------------------------------------------------*/
void LED_SW2812B_T1L_hold(void) 
{
		__nop(); 
//		__nop(); __nop(); __nop(); __nop();
//		__nop(); __nop(); __nop(); __nop(); __nop();
		
//		__nop(); __nop(); __nop(); __nop(); __nop();		

}


/*-----------------------------------------------------------------------
	0xggrrbb00
------------------------------------------------------------------------*/
void LED_SW2812B_show(u32 *pdata, u16 num, u8 close) 
{
	u16 i, j;
	u32 temp;

	for (i = 0; i < num; i++){

		//按照GRB的顺序

		temp = close ? *(pdata + i) : 0;		
		for (j = 0; j < 24; j++){							
			if(temp >= 0x80000000 ){		// 1			
				LED1_ON;	LED_SW2812B_T1H_hold();				
				LED1_OFF;	__nop();	/* LED_SW2812B_T1L_hold(); */				
			}else{			
				LED1_ON;	LED_SW2812B_T0H_hold();				
				LED1_OFF;	LED_SW2812B_T0L_hold();
			}
			temp <<= 1;
		}		
	}

}

/*-----------------------------------------------------------------------
	RESET
------------------------------------------------------------------------*/
void LED_SW2812B_reset(void) 
{
	LED1_OFF;
	LED_RESET_hold(52);

}




/*-----------------------------------------------------------------------
	100ms
	
------------------------------------------------------------------------*/
void LED_test(void) 
{
	static u32 temp[10] = {LED1_GREEN, LED1_RED, LED1_BLUE, LED1_WHITE, LED1_YELLOW, LED1_MAGENTA, LED1_CYAN, 0};

	static u16 cnt_100ms = 0;
	static u16 cnt_1s = 0;
	u16 i;
	u32 abc;

	cnt_100ms++;
	if(cnt_100ms >= 5){
		cnt_100ms = 0;
		cnt_1s++;
	}

	if(cnt_1s > 60){
		cnt_1s = 0;	
		temp[0] = LED1_GREEN;
		temp[1] = LED1_RED;
		temp[2] = LED1_BLUE;
		temp[3] = LED1_WHITE;
		temp[4] = LED1_YELLOW;
	
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 0);	
	}
	
	 if(cnt_1s < 5){			//逐个点亮
	
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, cnt_1s + 1, 1);

	}else if(cnt_1s < 10){	//逐个灭
	
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, cnt_1s -4, 0);

	}else if(cnt_1s < 15){

		if(cnt_1s % 2 == 0){
			LED_SW2812B_reset();
			LED_SW2812B_show(temp, 5, 1);
		}else{
			LED_SW2812B_reset();
			LED_SW2812B_show(temp, 5, 0);
		}
		
	}else if(cnt_1s == 15){		//
		temp[0] = 0;
		temp[1] = 0;
		temp[2] = 0;
		temp[3] = 0;
		temp[4] = 0;
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s < 20){

		temp[0] += 0x06000000;
		temp[1] += 0x06000000;
		temp[2] += 0x06000000;
		temp[3] += 0x06000000;
		temp[4] += 0x06000000;

		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);
		
	}else if(cnt_1s == 20){		//
		temp[0] = 0;
		temp[1] = 0;
		temp[2] = 0;
		temp[3] = 0;
		temp[4] = 0;
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);
	
	}else if(cnt_1s < 25){
	
		temp[0] += 0x00060000;
		temp[1] += 0x00060000;
		temp[2] += 0x00060000;
		temp[3] += 0x00060000;
		temp[4] += 0x00060000;
	
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);


	}else if(cnt_1s == 25){		//
		temp[0] = 0;
		temp[1] = 0;
		temp[2] = 0;
		temp[3] = 0;
		temp[4] = 0;
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s < 30){
	
		temp[0] += 0x00000600;
		temp[1] += 0x00000600;
		temp[2] += 0x00000600;
		temp[3] += 0x00000600;
		temp[4] += 0x00000600;
	
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);


	}else if(cnt_1s == 30){		//
		temp[0] = 0;
		temp[1] = 0;
		temp[2] = 0;
		temp[3] = 0;
		temp[4] = 0;
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s  < 35){
	
		temp[0] += 0x06060600;
		temp[1] += 0x06060600;
		temp[2] += 0x06060600;
		temp[3] += 0x06060600;
		temp[4] += 0x06060600;
	
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);


	}else if(cnt_1s == 35){		//

		temp[0] = LED1_GREEN;
		temp[1] = LED1_RED;
		temp[2] = LED1_BLUE;
		temp[3] = LED1_WHITE;
		temp[4] = LED1_YELLOW;
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s < 40){
		abc = temp[0];
		for(i = 0; i < 4; i++){
			temp[i] = temp[i+1];
		}	
		temp[4] = abc;
		
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);


	}else if(cnt_1s == 40){		//

		temp[0] = LED1_GREEN;
		temp[1] = 0;
		temp[2] = 0;
		temp[3] = 0;
		temp[4] = 0;
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s < 45){
		abc = temp[0];
		for(i = 0; i < 4; i++){
			temp[i] = temp[i+1];
		}	
		temp[4] = abc;
		
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);


	}else if(cnt_1s == 45){		//

		temp[0] = 0;
		temp[1] = LED1_RED;
		temp[2] = 0;
		temp[3] = 0;
		temp[4] = 0;
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s < 50){
		abc = temp[0];
		for(i = 0; i < 4; i++){
			temp[i] = temp[i+1];
		}	
		temp[4] = abc;
		
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s == 50){		//

		temp[0] = 0;
		temp[1] = 0;
		temp[2] = LED1_BLUE;
		temp[3] = 0;
		temp[4] = 0;
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s < 55){
		abc = temp[0];
		for(i = 0; i < 4; i++){
			temp[i] = temp[i+1];
		}	
		temp[4] = abc;
		
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s == 55){		//

		temp[0] = 0;
		temp[1] = 0;
		temp[2] = 0;
		temp[3] = LED1_WHITE;
		temp[4] = 0;
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);

	}else if(cnt_1s < 60){
		abc = temp[0];
		for(i = 0; i < 4; i++){
			temp[i] = temp[i+1];
		}	
		temp[4] = abc;
		
		LED_SW2812B_reset();
		LED_SW2812B_show(temp, 5, 1);



	}


}



 
