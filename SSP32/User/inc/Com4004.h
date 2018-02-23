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
#define RANDOM_MAX_NUM       	255          	/* 随机数最大值 */
#define RANDOM_MIN_NUM        	1              /* 随机数最小值 */
#define MAX_BUF_LEN		 	1024

//发送增加队列，新增定义send_message函数的返回结构体
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
	u8 r_length[4];	//remaining length最大有4个字节
	u8 r_length_len;//r_length[]数组实际有值的个数
}Fixheader;	//协议固定头部

typedef struct
{
	u8 version;
	u8 topic_lengthH;
	u8 topic_lengthL;
	u8 ext_message_id;

	u8 topic[300];
	u16 msgid;
	
	
}Variableheader;//协议可变头部


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
}DM;	//调光指令包含erase、duration

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
}TIME;	//action backlight中的time内容

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
}SC_sense;		//SC的传感器，即SPC上的ACS712

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
	
}ST_sense;		//ST的传感器，包含手势、环境光亮度、接近、环境光颜色、触摸按键值

typedef struct
{
	int temperature;	//SHT30温度，2byte表示，有正负
	u8 humidity;			//SHT30湿度
	u8  pm2_5_H;			//PM2.5 2bytes表示
	u8  pm2_5_L;
	u8  motion;	//运动传感器
	u8  presence;//人体检测
	u8  bt;			//光线强度
	u8  CO_H;		//CO浓度，2bytes
	u8  CO_L;
	u8  CO2_H;	//CO2浓度，2bytes
	u8  CO2_L;
	u8 	VOC;		//VOC
	u8 	smoke;	//烟雾
	//u8 	energy_diff;	
	u8 	human_sensing;//1位有人，0为无人，11左右两边均有人
}SS_sense;

typedef struct
{
	u8				MDID;		/* 设备存在的依据 *///用于汇报SLC/SPC的信息，模块ID
	union 	FLAG 		status;		//b0表示网关接收到sicp的心跳包回复
												//b1表示收到sicp回复channel状态，执行成功
												//b2表示收到sicp回复action cmd执行失败
	u8				ch1_status;
	u8				ch2_status;
	u8				ch3_status;
	u8				ch4_status;
	
	u8				firmware;		//固件版本
	u8				HWTtest;			//故障信息，详见SIDP 6.Device Addressing
	
}SLC;

typedef struct
{

	u8				MDID;			/* 设备存在的依据 */ //用于汇报SLC/SPC的信息，模块ID
	union 		FLAG status;		//b0表示网关接收到sicp的心跳包回复
	
	u8				ch1_status;
	u8				ch2_status;
	u8				ch3_status;
	u8				ch4_status;

	u8				firmware;		//固件版本	
	u8				HWTtest;		//故障信息，详见SIDP 6.Device Addressing
	
	u16				energy_consum;
	
}SPC;




#define SC_NUMS_OF_SS		5
#define ST_NUMS_OF_SS		20
#define SL_NUMS_OF_SC		15
#define SP_NUMS_OF_SC		15




typedef struct
{
	
	u8				legalFlag;		
	u8				heartBeatFlag;	/* 心跳广播标志，为1时表示正在广播心跳 */	
	
	u16				meshid;		/* 设备存在的依据 */
	char 				deviceid[12];

	u8				model;		//设备型号
	u8				firmware;	
	u8				HWTtest;		//故障信息，详见SIDP 6.Device Addressing

	SC_sense 			sense;

	u8				Ndevice;				//SC下挂有几个SLC/SPC
	SLC				slc[SL_NUMS_OF_SC];		//暂定，一个SC下最多挂15个slc
	SPC				spc[SP_NUMS_OF_SC];		//暂定，一个SC下最多挂15个spc

	union 		FLAG status;//b0表示网关接收到sicp的心跳包回复
	
}SC;



//typedef struct
//{

//	u8			type;		//type为1时表示配置类型为按键，为2时为手势

//	u8			key;			//第几个按键
//	u16			cond;		//手势动作(0xefe0)

//	u16			meshid;		//此为要配置的st的meshid,需自己查找所有st的deviceId来得到

//	u8 			boardid;		//mdid+channel
//	u8			action;
//	u8 			value;
//	
//}config_st_t;	//config st data struct


typedef union{
	
	u8 byte;
	struct{
	        u8 sended:1;		//已经发送
	        u8 configok:1;		//配置成功
	        u8 bit2:1;
	        u8 bit3:1;
	        u8 bit4:1;
		u8 bit5:1;
	        u8 bit6:1;
	        u8 bit7:1;

  	}bit;
	
} config_st_status_t;



//用来解析topic
typedef struct config_st_t {
	
	struct config_st_t *next;		
	
	config_st_status_t			flag;			//配置标志，如果该st全部配置成功，则删除该st设备的config列表
	u8 			msgid;				//用于确认接收配置成功


	u8			type;		//type为1时表示配置类型为按键，为2时为手势

	u8			key;			//第几个按键
	u16			cond;		//手势动作(0xefe0)

	u16			meshid;		//此为要配置的st的meshid,需自己查找所有st的deviceId来得到

	u8 			boardid;		//mdid+channel
	u8			action;
	u8 			value;
	
}config_st_t;	//config st data struct


#define	MPR121_KEYPAD_NUMS	3
#define	MPR121_LONG_TOUCH_INVALID			0x0A		/*  长按启动滑动功能无效  */			

typedef struct
{
	u8				legalFlag;		

	u16				meshid;		/* 设备存在的依据 */
	char 				deviceid[12];

	u8				model;		//设备型号
	u8				firmware;
	u8				HWTtest;		//故障信息，详见SIDP 6.Device Addressing

	ST_sense 			sense;

	union 		FLAG status;		//b0表示网关接收到sicp的心跳包回复
												//b1表示收到sicp回复状态，执行成功
												//b2表示收到sicp回复action cmd执行失败
	u8				ch1_status;
	u8				ch2_status;
	u8				ch3_status;
	u8				ch4_status;

	/* esh发送的滑动更新值需要保存 */
	u8				level1;
	u8				level2;

	/* 手势和独立 按键没有必要保存 */
	/* 滑动按键和长按启动的滑动功能的滑动值需要保存  */
//	u8				slipLevel[4];



	config_st_t		*config_head;
	config_st_t		*config_last;

	
}ST;


typedef union{
	
	u8 byte;
	struct{
	        u8 ble_en:1;		//1010连接mesh网络，正常工作
	        u8 tcp_en:1;		//4004建立tcp连接
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

	        u8 f_uart:1;			/* 与上级通讯离线标志 */
	        u8 f_ble:1;			/* 1010通讯故障，3min内检测时候接受到1010的状态帧、1010是否连接mesh网络并正常运行 */
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

/* 辅助检测故障 */
typedef union{
	
	u32 byte;
	struct{

	        u8 f_uart:1;			
	        u8 f_ble_network_status:1;		/* 接受到1010状态是置1 */
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

	int				model;		//设备型号
	int				firmware;	

	ss_malfunction_t	HWTtest;
	ss_maldetect_t		malDetect;


	char				macwifi[20];
	CS			 	config;
	SS_sense 	sense;
	SC				sc[SC_NUMS_OF_SS];//暂定，一个SS下最多挂5个sc
	ST				st[ST_NUMS_OF_SS];//暂定，一个SS下最多挂20个st

	u8 				alarm_level;

	ss_status_t		flag;
	
}SS;




typedef struct
{
	char		cond[100];
	u8			id;
	SL_AP		sl_ap[10];	//暂定一个SS下面最多一次收到10个SL action指令
	u8 			sl_num;
	SP_AP		sp_ap[10];	//暂定一个SS下面最多一次收到10个SP action指令
	u8 			sp_num;
}CSHP;	//config strategy hpst

typedef struct
{
	int				type;//type为1时表示配置类型为按键，为2时为手势
	char			target_id[12];
	int				key;
	char			cond[3];
	int 			mdid;
	int 			ch;
	char			action[2];
	int 			value;
	u16				meshid;//此为要配置的st的meshid,需自己查找所有st的deviceId来得到
}CST;	//config st data struct





typedef struct
{
	char sepid[12];
	char ch[2];
}DES;	//device status data struct


typedef struct
{
	char 			sepid[12];//回复payload中action里的sepid
	char 			sepid2[12];//回复payload中report里的sepid
	int 			MD;
	ST_sense	sts;
	struct	PX
	{
		u8 		isPX;//接收到的指令是PX接近传感
		int 	value;
	}px;
	struct	EG
	{
		u8 	isEG;	//接收到的指令是EG用电量
		int MD;		//module id;
		int 	value;
	}eg;
	struct	CP	//接收到的指令是CP触摸按键
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
		u8		isGT;//接收到的指令是GT手势识别
		int 	ch;
		char 	action[2];
		char 	topos[2];
		int	 	option_duration;
		int  	option_erase;
		int  	timeout;
	}gt;
	
}RT;	//real time reporting ST的运动信息，包含接近、用电量、触摸按键、手势识别




COM4004_EXT u8  		DMA_Rece_Buf[DMA_BUF_LEN];	   		/* DMA接收串口数据缓冲区 */
COM4004_EXT u16 	Usart1_Rec_Cnt;             			/* 本帧数据长度	*/
COM4004_EXT u8  		Usart1_Send_Buf[USART1_TXBUF_LEN];	/* 发送缓冲区 */
COM4004_EXT u16 	Usart1_Send_Length;				/* uart1要发送的长度 */
COM4004_EXT u16 	Usart1_Send_Cnt;					/* uart1实际已发送的长度计数 */
COM4004_EXT u16 	Usart1_Delay_Cnt;					/* 接收到4004的完整数据后从0开始计数，每2ms加1，当时间超过20ms时不能发送数据 */


COM4004_EXT Txmessage 	Txto4004;					//发送给4004的结构体缓存，每次发送都更新该数据结构
COM4004_EXT Rxmessage 	Rxfr4004;					//接收到4004的结构体缓存，每次接收后都更新
COM4004_EXT SS				ss;						//本ss管理的所有数据
COM4004_EXT AB 				ss_ab;				//action backlight数据缓存
COM4004_EXT CS				ss_cs;				//config ss数据缓存
COM4004_EXT AR				ss_ar;				//action refresh深度指令

COM4004_EXT RT				ss_rt;				//rt指令



COM4004_EXT union  FLAG 		UART1Flag1_;
#define UART1Flag1 				UART1Flag1_._flag_byte
#define rev_success				UART1Flag1_._flag_bit.bit0	//用于接收数据分析
#define ack_ar						UART1Flag1_._flag_bit.bit1	//用于回复action refresh刷新所有传感器
#define rev_ar1					UART1Flag1_._flag_bit.bit2	//接收到action/refresh刷新SS传感器指令
#define rev_ar2					UART1Flag1_._flag_bit.bit3	//接收到action/refresh刷新ST传感器指令

COM4004_EXT union  FLAG 		UART1Flag2_;
#define UART1Flag2 				UART1Flag2_._flag_byte
#define ack_ab					UART1Flag2_._flag_bit.bit0	//回复action backlight
#define ack_dr						UART1Flag2_._flag_bit.bit1	//回复data recent强制刷新传感器数据并获取
#define ack_cm					UART1Flag2_._flag_bit.bit2	//回复config mesh
#define rev_st_rt					UART1Flag2_._flag_bit.bit3	//接收到st发出检测到运动



COM4004_EXT u8 rev_buf[MAX_BUF_LEN];	//接收到4004的一包数据，包括包头BB BB 包尾0A 0A，及其他可能混加在包头之前包尾之后的数据
COM4004_EXT u8 ssp_buf[MAX_BUF_LEN];	//真正的4004的有效数据部分，去掉包头BB BB 包尾0A 0A及其他无效数据
COM4004_EXT u16 ssp_length;			//一包4004的数据长度，及协议中remaining length长度




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
	int flag;		/* 为1时表示设备列表有效 */

	int times;		/* 控制每DEVICE_LEGALITY_CHECK_TIME秒进行一次检查 */


/*----------------------------------------------------------------------------
	ss, sl,sp, st 均为数组类型，格式如下
	
	 [
	 	{
			"deviceID": "SLS1GS2F",
		},

		{
			"deviceID": "SLS1GS2F",
		}
	]
	
-----------------------------------------------------------------------------*/

	u8		scBroadcastRecvFlag[SC_NUMS_OF_SS];		/* 广播接收标志 */
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



//用来解析topic
typedef struct topic_t {
	
	struct topic_t *next;		
	
	char *string;		//字符串
	
	int 	len;			//字符串长度
	
}topic_t;


extern topic_t *topic_head;
extern topic_t *topic_last;


//‘/’与‘/’之间的字符串长度默认不超过100
#define 	SINGLE_STRING_LENGTH_OF_TOPIC		10


typedef struct
{

	char 	sepid[12];
	char 	action[4];

	u8	mdid_channel[16];

	u16	meshid;		//sepid对应的meshid
	u8	cmd;		//命令值
	
	u8  	topos;		//控制值
	u8  	duration;		//调光时间
	
	int 	type;			//qe中action/UR的type
	char 	code[10];			//qe中action/UR的code
//	char 	raw[SINGLE_STRING_LENGTH_OF_TOPIC];		//qe中action/UR的raw

	
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


