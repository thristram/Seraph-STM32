//*************************************************************
#ifndef  COM4004_H
#define  COM4004_H
//*************************************************************

#ifndef  _COM4004_GLOBAL
#define  COM4004_EXT  extern 
#else
#define  COM4004_EXT 
#endif
#include "sys.h"
#include "type.h"
#include "cJSON.h"


#define DMA_BUF_LEN 			1024
#define USART1_TXBUF_LEN 	1024
#define RANDOM_MAX_NUM       	255          	/* ��������ֵ */
#define RANDOM_MIN_NUM        	1              /* �������Сֵ */
#define MAX_BUF_LEN		 	1024

//�������Ӷ��У���������send_message�����ķ��ؽṹ��
typedef struct{
	u8 	*temp_send_buf;
	u16 tx_len;
}ret_struct;

typedef struct
{
	union 
	{
		struct
		{
		u8 message_type	:4;
		u8 dup_flag			:1;
		u8 qos_level		:2;
		u8 message_source:1;
		}first_ch_union;
		u8 first_ch_byte;
	}ch;
	u8 r_length[4];	//remaining length�����4���ֽ�
	u8 r_length_len;//r_length[]����ʵ����ֵ�ĸ���
}Fixheader;	//Э��̶�ͷ��

typedef struct
{
	u8 version;
	u8 topic_lengthH;
	u8 topic_lengthL;
	u8 ext_message_id;

	u8 topic[300];
	u16 msgid;
	
	
}Variableheader;//Э��ɱ�ͷ��


typedef struct
{
	Fixheader 			tx_fix_header;
	Variableheader 		tx_var_header;	
	u32				tx_payload_len;
	char 				tx_payload[1024];
	
}Txmessage;

typedef struct
{
	Fixheader 			rx_fix_header;
	Variableheader 		rx_var_header;
	u8 				rx_payload[1024];
}Rxmessage;

typedef struct
{
	int erase;
	int duration;
}DM;	//����ָ�����erase��duration

typedef struct
{
	int  seqid;
	char sepid[12];
	int  ch;
	char action[4];
	char topos[4];
	DM   option;
	int  stseq;
	int  timeout;
}SL_AP;//SL action/perform data struct

typedef struct
{
	int  seqid;
	char sepid[12];
	int  ch;
	char action[4];
	char topos[4];
	int  stseq;
	int  timeout;
}SP_AP;//SC action/perform data struct




typedef struct
{
	int in;
	int duration;
	int out;
	int blank;
}TIME;	//action backlight�е�time����

typedef struct
{
	char    sepid[12];
	int 		mode;
	char 		color[10][6];
	u8 			color_num;
	int 		density;
	TIME 		time;
	int 		speed;
	int 		display;
}AB;		//action/backlight data struct

typedef struct
{
	int 	hpst;
	int 	refr;
	char 	ssid[100];
	char	paswd[50];
	int		time;
	char 	city[20];
	char	country[20];
	char	region[20];
	char	hm[10];
	char	pr[10];
	char	visibility[10];
	char	sunrise[6];
	char	sunset[6];
	char	code[10];
	char	tp[10];
	char	index[10];
}CS;		//config/ss data struct

typedef struct
{
	u8 		channel;
	char 	ch[2];
}AR;		//action/re

typedef struct
{
	u8 		channel;
	char 	ch[2];
}DS;		//data/sync data struct


typedef struct
{
	u32 total_energy_consum;
	u16 total_current;
	u8  voltage;
}SC_sense;		//SC�Ĵ���������SPC�ϵ�ACS712

typedef struct
{
	//u8 brightness;
	u8 handgusture_H;
	u8 handgusture_L;
	u8 ambient_light;
	u8 proximity;
	u8 color_sense_H;
	u8 color_sense_M;
	u8 color_sense_L;
	u8 pad_value;
	
}ST_sense;		//ST�Ĵ��������������ơ����������ȡ��ӽ�����������ɫ����������ֵ

typedef struct
{
	int temperature;	//SHT30�¶ȣ�2byte��ʾ��������
	u8 humidity;			//SHT30ʪ��
	u8  pm2_5_H;			//PM2.5 2bytes��ʾ
	u8  pm2_5_L;
	u8  motion;	//�˶�������
	u8  presence;//������
	u8  bt;			//����ǿ��
	u8  CO_H;		//COŨ�ȣ�2bytes
	u8  CO_L;
	u8  CO2_H;	//CO2Ũ�ȣ�2bytes
	u8  CO2_L;
	u8 	VOC;		//VOC
	u8 	smoke;	//����
	//u8 	energy_diff;	
	u8 	human_sensing;//1λ���ˣ�0Ϊ���ˣ�11�������߾�����
}SS_sense;

typedef struct
{
	u8				MDID;		/* �豸���ڵ����� *///���ڻ㱨SLC/SPC����Ϣ��ģ��ID
	union 	FLAG 		status;		//b0��ʾ���ؽ��յ�sicp���������ظ�
												//b1��ʾ�յ�sicp�ظ�channel״̬��ִ�гɹ�
												//b2��ʾ�յ�sicp�ظ�action cmdִ��ʧ��
	u8				ch1_status;
	u8				ch2_status;
	u8				ch3_status;
	u8				ch4_status;
	
	u8				firmware;		//�̼��汾
	u8				HWTtest;			//������Ϣ�����SIDP 6.Device Addressing
	
}SLC;

typedef struct
{

	u8				MDID;			/* �豸���ڵ����� */ //���ڻ㱨SLC/SPC����Ϣ��ģ��ID
	union 		FLAG status;		//b0��ʾ���ؽ��յ�sicp���������ظ�
	
	u8				ch1_status;
	u8				ch2_status;
	u8				ch3_status;
	u8				ch4_status;

	u8				firmware;		//�̼��汾	
	u8				HWTtest;		//������Ϣ�����SIDP 6.Device Addressing
	
	u16				energy_consum;
	
}SPC;




#define SC_NUMS_OF_SS		5
#define ST_NUMS_OF_SS		20
#define SL_NUMS_OF_SC		15
#define SP_NUMS_OF_SC		15




typedef struct
{
	
	u8				legalFlag;		
	u8				heartBeatFlag;	/* �����㲥��־��Ϊ1ʱ��ʾ���ڹ㲥���� */	
	
	u16				meshid;		/* �豸���ڵ����� */
	char 				deviceid[12];

	u8				model;		//�豸�ͺ�
	u8				firmware;	
	u8				HWTtest;		//������Ϣ�����SIDP 6.Device Addressing

	SC_sense 			sense;

	u8				Ndevice;				//SC�¹��м���SLC/SPC
	SLC				slc[SL_NUMS_OF_SC];		//�ݶ���һ��SC������15��slc
	SPC				spc[SP_NUMS_OF_SC];		//�ݶ���һ��SC������15��spc

	union 		FLAG status;//b0��ʾ���ؽ��յ�sicp���������ظ�
	
}SC;



//typedef struct
//{

//	u8			type;		//typeΪ1ʱ��ʾ��������Ϊ������Ϊ2ʱΪ����

//	u8			key;			//�ڼ�������
//	u16			cond;		//���ƶ���(0xefe0)

//	u16			meshid;		//��ΪҪ���õ�st��meshid,���Լ���������st��deviceId���õ�

//	u8 			boardid;		//mdid+channel
//	u8			action;
//	u8 			value;
//	
//}config_st_t;	//config st data struct


typedef union{
	
	u8 byte;
	struct{
	        u8 sended:1;		//�Ѿ�����
	        u8 configok:1;		//���óɹ�
	        u8 bit2:1;
	        u8 bit3:1;
	        u8 bit4:1;
		u8 bit5:1;
	        u8 bit6:1;
	        u8 bit7:1;

  	}bit;
	
} config_st_status_t;



//��������topic
typedef struct config_st_t {
	
	struct config_st_t *next;		
	
	config_st_status_t			flag;			//���ñ�־�������stȫ�����óɹ�����ɾ����st�豸��config�б�
	u8 			msgid;				//����ȷ�Ͻ������óɹ�


	u8			type;		//typeΪ1ʱ��ʾ��������Ϊ������Ϊ2ʱΪ����

	u8			key;			//�ڼ�������
	u16			cond;		//���ƶ���(0xefe0)

	u16			meshid;		//��ΪҪ���õ�st��meshid,���Լ���������st��deviceId���õ�

	u8 			boardid;		//mdid+channel
	u8			action;
	u8 			value;
	
}config_st_t;	//config st data struct


#define	MPR121_KEYPAD_NUMS	3
#define	MPR121_LONG_TOUCH_INVALID			0x0A		/*  ������������������Ч  */			

typedef struct
{
	u8				legalFlag;		

	u16				meshid;		/* �豸���ڵ����� */
	char 				deviceid[12];

	u8				model;		//�豸�ͺ�
	u8				firmware;
	u8				HWTtest;		//������Ϣ�����SIDP 6.Device Addressing

	ST_sense 			sense;

	union 		FLAG status;		//b0��ʾ���ؽ��յ�sicp���������ظ�
												//b1��ʾ�յ�sicp�ظ�״̬��ִ�гɹ�
												//b2��ʾ�յ�sicp�ظ�action cmdִ��ʧ��
	u8				ch1_status;
	u8				ch2_status;
	u8				ch3_status;
	u8				ch4_status;

	/* esh���͵Ļ�������ֵ��Ҫ���� */
	u8				level1;
	u8				level2;

	/* ���ƺͶ��� ����û�б�Ҫ���� */
	/* ���������ͳ��������Ļ������ܵĻ���ֵ��Ҫ����  */
//	u8				slipLevel[4];



	config_st_t		*config_head;
	config_st_t		*config_last;

	
}ST;


typedef union{
	
	u8 byte;
	struct{
	        u8 ble_en:1;		//1010����mesh���磬��������
	        u8 tcp_en:1;		//4004����tcp����
	        u8 post_devinfo_ss:1;
	        u8 recv_devinfo_list:1;
	        u8 recv_config_ss:1;
		u8 sicp_broadcast_finish:1;
	        u8 request_config_st:1;
	        u8 recv_config_st:1;

  	}bit;
	
} ss_status_t;

typedef union{
	
	u32 byte;
	struct{

	        u8 f_uart:1;			/* ���ϼ�ͨѶ���߱�־ */
	        u8 f_ble:1;			/* 1010ͨѶ���ϣ�3min�ڼ��ʱ����ܵ�1010��״̬֡��1010�Ƿ�����mesh���粢�������� */
		u8 bit2:1;
		u8 f_MFI:1;
	        u8 f_SHT30:1;
	        u8 f_CO:1;
		u8 f_CO2:1;
	        u8 f_PM25:1;
			
	        u8 f_PYD1798_1:1;
	        u8 f_PYD1798_2:1;
	        u8 f_IR:1;
	        u8 f_SMOKE:1;
			
	        u8 f_VOC:1;
		u8 bit13:1;
		u8 bit14:1;
		u8 bit15:1;
		
		u8 bit16:1;
		u8 bit17:1;
		u8 bit18:1;
		u8 bit19:1;
		u8 bit20:1;
		u8 bit21:1;
		u8 bit22:1;
		u8 bit23:1;
		
		u8 bit24:1;
		u8 bit25:1;
		u8 bit26:1;
		u8 bit27:1;
		u8 bit28:1;
		u8 bit29:1;
		u8 bit30:1;
		u8 bit31:1;

  	}bit;
	
} ss_malfunction_t;

/* ���������� */
typedef union{
	
	u32 byte;
	struct{

	        u8 f_uart:1;			
	        u8 f_ble_network_status:1;		/* ���ܵ�1010״̬����1 */
		u8 bit2:1;
		u8 f_MFI:1;
	        u8 f_SHT30:1;
	        u8 f_CO:1;
		u8 f_CO2:1;
	        u8 f_PM25:1;
			
	        u8 f_PYD1798_1:1;
	        u8 f_PYD1798_2:1;
	        u8 f_IR:1;
	        u8 f_SMOKE:1;
			
	        u8 f_VOC:1;
		u8 bit13:1;
		u8 bit14:1;
		u8 bit15:1;
		
		u8 bit16:1;
		u8 bit17:1;
		u8 bit18:1;
		u8 bit19:1;
		u8 bit20:1;
		u8 bit21:1;
		u8 bit22:1;
		u8 bit23:1;
		
		u8 bit24:1;
		u8 bit25:1;
		u8 bit26:1;
		u8 bit27:1;
		u8 bit28:1;
		u8 bit29:1;
		u8 bit30:1;
		u8 bit31:1;

  	}bit;
	
} ss_maldetect_t;

typedef struct
{
	u16				meshid;
	char 				deviceid[12];

	int				model;		//�豸�ͺ�
	int				firmware;	

	ss_malfunction_t	HWTtest;
	ss_maldetect_t		malDetect;


	char				macwifi[20];
	CS			 	config;
	SS_sense 	sense;
	SC				sc[SC_NUMS_OF_SS];//�ݶ���һ��SS������5��sc
	ST				st[ST_NUMS_OF_SS];//�ݶ���һ��SS������20��st

	u8 				alarm_level;

	ss_status_t		flag;
	
}SS;




typedef struct
{
	char		cond[100];
	u8			id;
	SL_AP		sl_ap[10];	//�ݶ�һ��SS�������һ���յ�10��SL actionָ��
	u8 			sl_num;
	SP_AP		sp_ap[10];	//�ݶ�һ��SS�������һ���յ�10��SP actionָ��
	u8 			sp_num;
}CSHP;	//config strategy hpst

typedef struct
{
	int				type;//typeΪ1ʱ��ʾ��������Ϊ������Ϊ2ʱΪ����
	char			target_id[12];
	int				key;
	char			cond[3];
	int 			mdid;
	int 			ch;
	char			action[2];
	int 			value;
	u16				meshid;//��ΪҪ���õ�st��meshid,���Լ���������st��deviceId���õ�
}CST;	//config st data struct





typedef struct
{
	char sepid[12];
	char ch[2];
}DES;	//device status data struct


typedef struct
{
	char 			sepid[12];//�ظ�payload��action���sepid
	char 			sepid2[12];//�ظ�payload��report���sepid
	int 			MD;
	ST_sense	sts;
	struct	PX
	{
		u8 		isPX;//���յ���ָ����PX�ӽ�����
		int 	value;
	}px;
	struct	EG
	{
		u8 	isEG;	//���յ���ָ����EG�õ���
		int MD;		//module id;
		int 	value;
	}eg;
	struct	CP	//���յ���ָ����CP��������
	{
		u8 		isCP;
		int 	ch;
		char 	action[2];
		char 	topos[2];
		int	 	option_duration;
		int  	option_erase;
		int  	timeout;
	}cp;
	struct	GT
	{
		u8		isGT;//���յ���ָ����GT����ʶ��
		int 	ch;
		char 	action[2];
		char 	topos[2];
		int	 	option_duration;
		int  	option_erase;
		int  	timeout;
	}gt;
	
}RT;	//real time reporting ST���˶���Ϣ�������ӽ����õ�������������������ʶ��




COM4004_EXT u8  		DMA_Rece_Buf[DMA_BUF_LEN];	   		/* DMA���մ������ݻ����� */
COM4004_EXT u16 	Usart1_Rec_Cnt;             			/* ��֡���ݳ���	*/
COM4004_EXT u8  		Usart1_Send_Buf[USART1_TXBUF_LEN];	/* ���ͻ����� */
COM4004_EXT u16 	Usart1_Send_Length;				/* uart1Ҫ���͵ĳ��� */
COM4004_EXT u16 	Usart1_Send_Cnt;					/* uart1ʵ���ѷ��͵ĳ��ȼ��� */
COM4004_EXT u16 	Usart1_Delay_Cnt;					/* ���յ�4004���������ݺ��0��ʼ������ÿ2ms��1����ʱ�䳬��20msʱ���ܷ������� */


COM4004_EXT Txmessage 	Txto4004;					//���͸�4004�Ľṹ�建�棬ÿ�η��Ͷ����¸����ݽṹ
COM4004_EXT Rxmessage 	Rxfr4004;					//���յ�4004�Ľṹ�建�棬ÿ�ν��պ󶼸���
COM4004_EXT SS				ss;						//��ss�������������
COM4004_EXT AB 				ss_ab;				//action backlight���ݻ���
COM4004_EXT CS				ss_cs;				//config ss���ݻ���
COM4004_EXT AR				ss_ar;				//action refresh���ָ��

COM4004_EXT RT				ss_rt;				//rtָ��



COM4004_EXT union  FLAG 		UART1Flag1_;
#define UART1Flag1 				UART1Flag1_._flag_byte
#define rev_success				UART1Flag1_._flag_bit.bit0	//���ڽ������ݷ���
#define ack_ar						UART1Flag1_._flag_bit.bit1	//���ڻظ�action refreshˢ�����д�����
#define rev_ar1					UART1Flag1_._flag_bit.bit2	//���յ�action/refreshˢ��SS������ָ��
#define rev_ar2					UART1Flag1_._flag_bit.bit3	//���յ�action/refreshˢ��ST������ָ��

COM4004_EXT union  FLAG 		UART1Flag2_;
#define UART1Flag2 				UART1Flag2_._flag_byte
#define ack_ab					UART1Flag2_._flag_bit.bit0	//�ظ�action backlight
#define ack_dr						UART1Flag2_._flag_bit.bit1	//�ظ�data recentǿ��ˢ�´��������ݲ���ȡ
#define ack_cm					UART1Flag2_._flag_bit.bit2	//�ظ�config mesh
#define rev_st_rt					UART1Flag2_._flag_bit.bit3	//���յ�st������⵽�˶�



COM4004_EXT u8 rev_buf[MAX_BUF_LEN];	//���յ�4004��һ�����ݣ�������ͷBB BB ��β0A 0A�����������ܻ���ڰ�ͷ֮ǰ��β֮�������
COM4004_EXT u8 ssp_buf[MAX_BUF_LEN];	//������4004����Ч���ݲ��֣�ȥ����ͷBB BB ��β0A 0A��������Ч����
COM4004_EXT u16 ssp_length;			//һ��4004�����ݳ��ȣ���Э����remaining length����




typedef enum
{
	GESTURE_REPORT = 1,
	KEYPAD_REPORT,
	SLIPPAD_REPORT,
	
}REPORT_TYPE;	



typedef enum
{
	DEVICE_NULL = 0,			
	DEVICE_SC,			
	DEVICE_ST,		
	DEVICE_SL,
	DEVICE_SP,
	DEVICE_SS,
	
	
}DEVICE_TYPE;	


typedef enum
{
	SSP_POST = 1,			
	SSP_GET,		
	
}SSP_SEND_TYPE;	



#define 	RECEIPT_CODE_SUCCESS		0x0200000
#define 	RECEIPT_CODE_ANALYZE_OK	0x0200001

#define 	RECEIPT_CODE_FAILED			0x0400000
#define 	RECEIPT_CODE_ANALYZE_ERROR	0x0400001

#define 	RECEIPT_CODE_ERROR			0x0401F08



#define 	DEVICE_LEGALITY_CHECK_TIME		5


typedef struct
{	
	int flag;		/* Ϊ1ʱ��ʾ�豸�б���Ч */

	int times;		/* ����ÿDEVICE_LEGALITY_CHECK_TIME�����һ�μ�� */


/*----------------------------------------------------------------------------
	ss, sl,sp, st ��Ϊ�������ͣ���ʽ����
	
	 [
	 	{
			"deviceID": "SLS1GS2F",
		},

		{
			"deviceID": "SLS1GS2F",
		}
	]
	
-----------------------------------------------------------------------------*/

	u8		scBroadcastRecvFlag[SC_NUMS_OF_SS];		/* �㲥���ձ�־ */
	u8		stBroadcastRecvFlag[ST_NUMS_OF_SS];

	u8		scDeviceID[SC_NUMS_OF_SS][4];
	u8		stDeviceID[ST_NUMS_OF_SS][4];


	int		scSize;
	int		stSize;		

	cJSON	*sc;
	cJSON	*st;
	cJSON	*sl;
	cJSON	*sp;

}devList_t;	//alarm 

extern devList_t devList;



//��������topic
typedef struct topic_t {
	
	struct topic_t *next;		
	
	char *string;		//�ַ���
	
	int 	len;			//�ַ�������
	
}topic_t;


extern topic_t *topic_head;
extern topic_t *topic_last;


//��/���롮/��֮����ַ�������Ĭ�ϲ�����100
#define 	SINGLE_STRING_LENGTH_OF_TOPIC		10


typedef struct
{

	char 	sepid[12];
	char 	action[4];

	u8	mdid_channel[16];

	u16	meshid;		//sepid��Ӧ��meshid
	u8	cmd;		//����ֵ
	
	u8  	topos;		//����ֵ
	u8  	duration;		//����ʱ��
	
	int 	type;			//qe��action/UR��type
	char 	code[10];			//qe��action/UR��code
//	char 	raw[SINGLE_STRING_LENGTH_OF_TOPIC];		//qe��action/UR��raw

	
}qe_t;		




extern qe_t ssp_qe;














u16 ssp_get_message_id(void);


COM4004_EXT void deal_config_ss(u8 *buf);
COM4004_EXT void send_config_ss(void);
COM4004_EXT void deal_deepsyn_config_ss(u8 *buf1,u8 *buf2);

COM4004_EXT void deal_action_backlight(u8 *buf);
COM4004_EXT void deal_action_refresh(u8 *buf);

COM4004_EXT void deal_config_mesh(u8 *buf);
COM4004_EXT void deal_config_stragy_hpst(u8 *buf);
COM4004_EXT void deal_config_strategy_htsp(u8 *buf);		


void send_rt(void);


void ssp_action_learn_ir(void);


u8 random(u8 xxx);
int mystrcmp(unsigned char *str1,const unsigned char *str2);
void mystrncat(char *desc,char *src,u32 len);




void ssp_data_sync(SSP_SEND_TYPE type);


void ssp_device_info_ss(SSP_SEND_TYPE type, u16 msgid);
void ssp_device_info_sub_response(u16 msgid);

void ssp_device_status_post(DEVICE_TYPE devType, u8 i);
void ssp_device_status_response(u8 *buf);

void ssp_device_malfunction_response(u16 msgid);
void ssp_device_malfunction_post(DEVICE_TYPE devType, u8 num, u8 mdid);
void ssp_device_malfunction_detect(void);


void ssp_device_list_recv(u8 *payload);


void ssp_energy_consum_post(u8 sc_num, u8 addr);
void ssp_motion_detect_post(void);
void ssp_smoke_detect_post(u8 value);
void ssp_gesture_or_keypad_post(u8 st_num, REPORT_TYPE type, u8 keyPad, u16 value);

void ssp_recepit_response(int code);
void ssp_check_device_legality(void);


void recv_4004_analyze(void);
void ssp_send_message(u8 first_ch_byte,u8 version,u8 topic_len,char *topic, u16 msgid,u8 ext_message_id,char *payload);



void ssp_send_data_active(void);


#endif


