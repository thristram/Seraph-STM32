
#ifndef __SENSORS_UART_H
#define __SENSORS_UART_H

#ifndef  _SENSORS_UART_GLOBAL
#define  _SENSORS_UART_GLOBAL  extern 
#else
#define  _SENSORS_UART_GLOBAL 
#endif

#include "sys.h"










/*-----------------------------------------------------------------------

	CO2 uart 发送说明
	有多条命令需要发送时，每隔100ms发送一条命令。
	读取命令每1s发送一次
	共有五条命令，各个命令相互独立，当同时出现多个命令需要发送时，
	优先级如下:
	setrFlag > atuoFlag > spanFlag > zeroFlag > readFlag
	理论上，spanFlag和zeroFlag不能同时出现，因为发送这两个命令都需要
	相应的浓度环境。
	每条命令的发送时间为9*10/9600=10ms.
	

	命令标志说明:
		0: 表示不发送
		1: 表示需要发送该命令


	sendFlag标志说明:
		0: 表示没有发送
		1: 表示正在发送命令，开始发送进程，发送完成回到0状态
		
//		2: 表示该命令发送完成，正在等待一定的延迟(50ms延迟使用的是定时器中断的定时，
//			并非是直接等待延迟)或等待回复完成(实际中传感器会马上回复，
//			并且回复时间为10ms左右)，延迟时间到后表示该发送进程结束，回到0状态。

			

------------------------------------------------------------------------*/

#define CO2_TXRX_BUF_LEN	12
#define CO2_TXRX_DATA_LEN	9

typedef struct
{

	uint8_t 	readFlag;		/* 1、读标志 */
	uint8_t 	readCtrl;		/* 控制每1s读取一次数据  */	
	uint16_t 	readValue;		/* 读取的浓度值  */

	uint8_t 	zeroFlag;		/* 2、校准标志*/

	uint8_t 	spanFlag;		/* 3、 */
	uint8_t 	spanHigh;		/* SPAN高字节 */
	uint8_t 	spanLow;		/* SPAN低字节 */

	uint8_t 	atuoFlag;		/* 4、自动校准零点标志 */
	uint8_t 	atuoValue;		/* 自动校准开关 */

	uint8_t 	setrFlag;		/* 5、设置量程标志 */
	uint8_t 	setrHigh;		/* 设置量程高字节 */
	uint8_t 	setrLow;		/* 设置量程低字节 */


	uint8_t		txLen;			/* 需要发送的字节数 */
	uint8_t		rxLen;
	uint8_t		hasTxLen;		/* 已经发送的字节数  */
	uint8_t		sendFlag;		/* 正在发送的的标志  */


	
	uint8_t		txBuf[CO2_TXRX_BUF_LEN];	
	uint8_t		rxBuf[CO2_TXRX_BUF_LEN];

}CO2_uart_t;



extern CO2_uart_t CO2_uart;




void CO2_usart3_init(void);
void CO2_usart3_send(void);

void CO2_zero_config(void);
void CO2_atuo_config(uint16_t setrValue);
void CO2_setr_config(uint16_t setrValue);
void CO2_SPAN_config(uint16_t spanValue);





#endif




















