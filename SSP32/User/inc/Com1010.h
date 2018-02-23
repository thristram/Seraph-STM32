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


/*------------------ message id���� --------------------------------
cmd-data��
st1~st20�ֱ�Ϊ0~19
cmd-refresh:
st1~st20�ֱ�Ϊ20~39
sc1~sc5�ֱ�Ϊ40~44
cmd-status:
st1~st20�ֱ�Ϊ50~69
sc1~sc5�ֱ�Ϊ70~74

action-command:
sc1~sc5�ֱ�Ϊ75~79
quick-event����action-command��
st1~st20�ֱ�Ϊ80~99
sc1~sc5�ֱ�Ϊ100~104 	��ģ��
sc1~sc5�ֱ�Ϊ105~109	��ģ��
led-command:
st1~st20�ֱ�Ϊ110~119

alert-command:
st1~st20�ֱ�Ϊ120~139
sc1~sc5�ֱ�Ϊ140~144
malfuncrtion command:

Gateway	Mesh	ID	Broadcasting:
0x9E 158

SS���յ�qe CP����󣬷���0x56�����ST��ST����action�����SC�Կ���SLC��SPC����ʱ�̶�message idΪ0x9F������0x08�첽֪ͨSS
SLC��SLC������ɺ�SC����AA 05��SS����ʱ��message id����0x9F��
SS���յ��󣬸����첽֪ͨ����Ӧ��SC��������device status(����SLC��SPC״̬)��eSH
0x9F

-------------------------------------------------------------------*/




#define SICP_SEND_HEARTBEAT_TIME	30


#define SICP_BROADCAST_LOOP_NUMS		5			/* �豸�㲥ѭ������*/





typedef struct
{
	u8 signal;
	u8 connect;
	u8 phones;
	u8 host_meshid_H;
	u8 host_meshid_L;
	
} NET_Status;			//1010����״̬֡network status reporting


typedef union{
	
	u8 byte;
	struct{
	        u8 recvFlag:1;					/* ���ձ�־ */
	        u8 sensorsFreshCmdSended:1;	/* ssp�յ�data/recent��sicp����ˢ�´������������ */
	        u8 bit2:1;
	        u8 bit3:1;
	        u8 bit4:1;
		u8 bit5:1;
	        u8 bit6:1;
	        u8 bit7:1;

  	}bit;
	
} sicp_status_t;


/***************ȫ�ֱ�������**************************/

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
	
} SICP_Message;		//����SIDPЭ�鶨��SICP_Message


/* SICP ����Ҫ��������ݻ��� */
typedef struct
{
	sicp_status_t status;

	u8 rxBuf[SICP_RX_BUF_LEN];
	u8 txBuf[SICP_TX_BUF_LEN];
	
} SICP_handle_t;		

extern SICP_handle_t SICP_handle;



COM1010_EXT u8  sicp_refr;			//SSˢ���¹�SC��ST�Ĵ�������ʱ����������esh���͵�consig ss�õ�

COM1010_EXT NET_Status ns;		//1010��network status reporting�������ݽṹ





/**************��������*******************************/
COM1010_EXT u8 Check_Sum(u8 *buf,u8 length);
COM1010_EXT void sicp_send_receipt(u8 type,u8 send_message_id,u16 send_mesh_id);

COM1010_EXT void sicp_cmd_refresh(void);
COM1010_EXT void sicp_cmd_data(void);
COM1010_EXT void sicp_led_cmd(void);




typedef enum
{
    GET_SERIES_TO_SAVE = 0,		/* ��ȡ���Ա����λ�� */
    GET_SERIES_WHEN_EXSIT ,		/* ��ȡ�Ѿ������λ�� */


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



