//*************************************************************
#ifndef  COM1010_H
#define  COM1010_H
//*************************************************************

#ifndef  _COM1010_GLOBAL
#define  COM1010_EXT  extern
#else
#define  COM1010_EXT
#endif
#include "sys.h"
#include "type.h"
#include "cJSON.h"


/*------------------ message id定义 --------------------------------
cmd-data：
st1~st20分别为0~19
cmd-refresh:
st1~st20分别为20~39
sc1~sc5分别为40~44
cmd-status:
st1~st20分别为50~69
sc1~sc5分别为70~74

action-command:
sc1~sc5分别为75~79
quick-event发送action-command：
st1~st20分别为80~99
sc1~sc5分别为100~104 	单模块
sc1~sc5分别为105~109	多模块
led-command:
st1~st20分别为110~119

alert-command:
st1~st20分别为120~139
sc1~sc5分别为140~144
malfuncrtion command:

Gateway	Mesh	ID	Broadcasting:
0x9E 158

SS接收到qe CP命令后，发送0x56命令给ST，ST发送action命令给SC以控制SLC或SPC，此时固定message id为0x9F。并且0x08异步通知SS
SLC或SLC调节完成后SC发送AA 05给SS，此时的message id就是0x9F。
SS接收到后，根据异步通知得相应的SC，并发送device status(包括SLC和SPC状态)给eSH
0x9F

-------------------------------------------------------------------*/




#define SICP_SEND_HEARTBEAT_TIME	30


#define SICP_BROADCAST_LOOP_NUMS		5			/* 设备广播循环次数*/





typedef struct
{
	u8 signal;
	u8 connect;
	u8 phones;
	u8 host_meshid_H;
	u8 host_meshid_L;
	
} NET_Status;			//1010网络状态帧network status reporting


typedef union{
	
	u8 byte;
	struct{
	        u8 recvFlag:1;					/* 接收标志 */
	        u8 sensorsFreshCmdSended:1;	/* ssp收到data/recent，sicp发送刷新传感器数据完成 */
	        u8 bit2:1;
	        u8 bit3:1;
	        u8 bit4:1;
		u8 bit5:1;
	        u8 bit6:1;
	        u8 bit7:1;

  	}bit;
	
} sicp_status_t;


/***************全局变量定义**************************/

#define SICP_RX_BUF_LEN			USART2_RX_BUF_LEN	
#define SICP_TX_BUF_LEN			USART2_TX_BUF_LEN	

typedef struct
{
	u8 frame_h1;
	u8 frame_h2;
	u8 message_id;
	u8 mesh_id_H;
	u8 mesh_id_L;
	u8 payload[SICP_TX_BUF_LEN];
	
} SICP_Message;		//根据SIDP协议定义SICP_Message


/* SICP 中需要处理的数据缓存 */
typedef struct
{
	sicp_status_t status;

	u8 rxBuf[SICP_RX_BUF_LEN];
	u8 txBuf[SICP_TX_BUF_LEN];
	
} SICP_handle_t;		

extern SICP_handle_t SICP_handle;



COM1010_EXT u8  sicp_refr;			//SS刷新下挂SC和ST的传感器计时，该数据由esh发送的consig ss得到

COM1010_EXT NET_Status ns;		//1010的network status reporting缓存数据结构





/**************函数声明*******************************/
COM1010_EXT u8 Check_Sum(u8 *buf,u8 length);
COM1010_EXT void sicp_send_receipt(u8 type,u8 send_message_id,u16 send_mesh_id);

COM1010_EXT void sicp_cmd_refresh(void);
COM1010_EXT void sicp_cmd_data(void);
COM1010_EXT void sicp_led_cmd(void);




typedef enum
{
    GET_SERIES_TO_SAVE = 0,		/* 获取可以保存的位置 */
    GET_SERIES_WHEN_EXSIT ,		/* 获取已经保存的位置 */


} GET_SERIES_WAY;


typedef enum
{
    GET_SERIES_SC = 0,
    GET_SERIES_ST,
    GET_SERIES_SL,
    GET_SERIES_SP,

} GET_SERIES_OBJECT;

typedef enum
{
    GET_SERIES_ERROR = 0Xff,

} GET_SERIES_RESULT;




extern u8 BIT[8];


void sicp_handle_clear(void);

void sicp_recv_analyze(void);


u8 get_series_number(GET_SERIES_WAY way, GET_SERIES_OBJECT object, u16 meshid, char *deviceid);
u8 get_device_number(GET_SERIES_OBJECT object, u16 meshid);

void sicp_config_st_send(void);
void sicp_alarm_cmd(void);


u8 sicp_get_message_id(void);

void sicp_recv_device_info(u16 meshid);
void sicp_ble_ctrl_cmd(u8 type, u16 mesh_id);

void sicp_broadcast_loop(void);

int string_toDec(const char *str);

void sicp_send_heartbeat(void);

unsigned char  string_touchar(const char *str);


unsigned char  string_tohex2(const char *str);
void sicp_send_message(SICP_Message *tx, u8 pay_len, u8 reSendFlag);


void reSendList_send(void);

/* main.c */
extern void system_init(void);

u8 XOR_Check(u8 *buf, u16 length);

#endif



