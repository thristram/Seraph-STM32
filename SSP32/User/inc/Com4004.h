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

#define DMA_Rec_Len 		1024
#define Send_Len 				1024
#define MAX              255            //��������ֵ
#define MIN              1              //�������Сֵ
#define MAX_BUF_LEN			 1024

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
	u8 r_length[4];
	u8 r_length_len;//r_length[]����ʵ����ֵ�ĸ���
}Fixheader;

typedef struct
{
	u8 version;
	u8 topic_lengthH;
	u8 topic_lengthL;
	u8 topic[300];
	u8 message_id_H;
	u8 message_id_L;
	u8 ext_message_id;
}Variableheader;

typedef enum 
{
	RESERCED,
	CONNETC,
	GET,
	POST,
	PUT,
	DELETE,
	OTA,
}_REQUEST_;

typedef enum 
{
	ACTION_PERPORM,	//action_performָ��ִ�����
	ACTION_REFRESH,
	ACTION_BACKLIGHT,
	CONFIG_SS,		//ֱ�ӻظ����ջ�ִ
	DATA_SYNC,
	POST_DATA_SYNC,//�������ʹ���������
	DATA_RECENT,
	DATA_IR,			//�ظ��ɹ�����IR����
	BAD_REQUEST,
	UNAUTHORIZED,
	FORBIDDEN,
	NOT_FOUND,
	REQUEST_TIMEOUT,
	INTERNAL_SERVER_ERROR,
	SERVICE_UNAVAILABLE,
	VERSION_NOT_SUPPORTED,
}_REPLY_;



typedef struct
{
	Fixheader 			tx_fix_header;
	Variableheader 	tx_var_header;	
	char 						tx_payload[1024];
}Txmessage;

typedef struct
{
	Fixheader 			rx_fix_header;
	Variableheader 	rx_var_header;	
	u8 							rx_payload[1024];
}Rxmessage;

typedef struct
{
	int erase;
	int duration;
}DM;
typedef struct
{
	int  seqid;
	char sepid[8];
	int  ch;
	char action[2];
	char topos[4];
	DM   option;
	int  stseq;
	int  timeout;
}SL_AP;//SL action/perform data struct

typedef struct
{
	int  seqid;
	char sepid[8];
	int  ch;
	char action[4];
	char topos[4];
	int  stseq;
	int  timeout;
}SP_AP;//SC action/perform data struct

typedef struct
{
	u8		qos;				//qos = sl_num + sp_num
	SL_AP	sl_ap[10];	//�ݶ�һ��SS�������һ���յ�10��SL actionָ��
	u8 		sl_num;
	SP_AP	sp_ap[10];	//�ݶ�һ��SS�������һ���յ�10��SP actionָ��
	u8 		sp_num;
}AP;		//action/perform data struct


typedef struct
{
	int in;
	int duration;
	int out;
	int blank;
}TIME;

typedef struct
{
	char    sepid[8];
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
	int 	type;
	char 	code[20];
	char	address[10];
	char	other[10];
	char	raw[800];
}IR;

typedef struct
{
	u32 total_energy_consum;
	u16 total_current;
	u8  voltage;
}SC_sense;

typedef struct
{
	u8 brightness;
	u8 handgusture_H;
	u8 handgusture_L;
	u8 ambient_light;
	u8 proximity;
	u8 color_sense_H;
	u8 color_sense_M;
	u8 color_sense_L;
	u8 pad_value;
}ST_sense;

typedef struct
{
	int temperature;
	u16 humidity;
	u8  pm2_5_H;
	u8  pm2_5_L;
	u8  motion;	//�˶�������
	u8  presence;//������
	u8  bt;			//����ǿ��
	u8  CO_H;
	u8  CO_L;
	u8  CO2_H;
	u8  CO2_L;
	u8 	VOC;
	u8 	smoke;
	u8 	energy_diff;
	u8 	human_sensing;//1λ���ˣ�0Ϊ���ˣ�11�������߾�����
}SS_sense;

typedef struct
{
	u16				meshid;
	char 			deviceid[8];
	char			firmware[3];
	int				HWTtest;
	char			model[3];//�豸�ͺ�
	char			coord[10];//�豸�ڷ����λ��
	char			macwifi[17];
	int				MDID;//���ڻ㱨SLC/SPC����Ϣ��ģ��ID
	u8				posted;//Ϊ1ʱ��ʾ�Ѿ���ss���͸�esh,Ϊ0��ʾ��û������
	union 		FLAG status;//b0��ʾ���ؽ��յ�sicp���������ظ�
												//b1��ʾ�յ�sicp�ظ�channel״̬��ִ�гɹ�
												//b2��ʾ�յ�sicp�ظ�action cmdִ��ʧ��
	u8				ch1_status;
	u8				ch2_status;
	u8				ch3_status;
	u8				ch4_status;
}SLC;

typedef struct
{
	u16				meshid;
	char 			deviceid[8];
	char			firmware[3];
	int				HWTtest;
	char			model[3];//�豸�ͺ�
	char			coord[10];//�豸�ڷ����λ��
	char			macwifi[17];
	int				MDID;//���ڻ㱨SLC/SPC����Ϣ��ģ��ID
	u8				posted;//Ϊ1ʱ��ʾ�Ѿ���ss���͸�esh,Ϊ0��ʾ��û�����
	union 		FLAG status;//b0��ʾ���ؽ��յ�sicp���������ظ�
	u8				ch1_status;
	u8				ch2_status;
	u8				ch3_status;
	u8				ch4_status;
	u16				energy_consum;
}SPC;

typedef struct
{
	u16				meshid;
	char 			deviceid[8];
	char			firmware[3];
	int				HWTtest;
	char			model[3];//�豸�ͺ�
	char			coord[10];//�豸�ڷ����λ��
	char			macwifi[17];
	SC_sense 	sense;
	int				Ndevice;//SC�¹��м���SLC/SPC
	SLC				slc[15];//�ݶ���һ��SC������15��slc
	SPC				spc[15];//�ݶ���һ��SC������15��spc
	u8				posted;//Ϊ1ʱ��ʾ�Ѿ���ss���͸�esh,Ϊ0��ʾ��û�����
	union 		FLAG status;//b0��ʾ���ؽ��յ�sicp���������ظ�
}SC;

typedef struct
{
	u16				meshid;
	char 			deviceid[8];
	char			firmware[3];
	int				HWTtest;
	char			model[3];//�豸�ͺ�
	char			coord[10];//�豸�ڷ����λ��
	char			macwifi[17];
	ST_sense 	sense;
	u8				posted;//Ϊ1ʱ��ʾ�Ѿ���ss���͸�esh,Ϊ0��ʾ��û�
	union 		FLAG status;//b0��ʾ���ؽ��յ�sicp���������ظ�
												//b1��ʾ�յ�sicp�ظ�״̬��ִ�гɹ�
												//b2��ʾ�յ�sicp�ظ�action cmdִ��ʧ��
	u8				ch1_status;
	u8				ch2_status;
	u8				ch3_status;
	u8				ch4_status;
}ST;

typedef struct
{
	u16				meshid;
	char 			deviceid[8];
	char			firmware[2];
	int				HWTtest;
	char			model[2];//�豸�ͺ�
	char			coord[10];//�豸�ڷ����λ��
	char			macwifi[17];
	CS			 	config;
	SS_sense 	sense;
	IR				ir;
	SC				sc[5];//�ݶ���һ��SS������5��sc
	ST				st[20];//�ݶ���һ��SS������20��st
	u8				posted;//Ϊ1ʱ��ʾ�Ѿ���ss���͸�esh,Ϊ0��ʾ��û��
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
	char			target_id[10];
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
	char sepid[8];
	char ch[2];
}DES;	//device status data struct

typedef struct
{
	char sepid[8];
	int  MDID;
	int  CH;
	char action[2];
	int  topos;
	int  type;
	char code[6];
	char raw[500];
}QE;

typedef struct
{
	char 			sepid[8];
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
	struct	CP
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
}RT;

typedef struct
{
	char sepid[8];
	int  level;
}ALARM;


COM4004_EXT u8  DMA_Rece_Buf[DMA_Rec_Len];	   //DMA���մ������ݻ�����
COM4004_EXT u16 Usart1_Rec_Cnt;             	//��֡���ݳ���	
COM4004_EXT u8  USART1_Send_Buf[Send_Len];		//���ͻ�����
COM4004_EXT u16 Usart1_Send_Length;
COM4004_EXT u16 Usart1_Send_Cnt;
COM4004_EXT u8  Usart1_Send_Done;


COM4004_EXT Txmessage Txto4004;
COM4004_EXT Rxmessage Rxfr4004;
COM4004_EXT SS				ss;
COM4004_EXT AP 				ss_ap;				//action peform
COM4004_EXT u8				ss_ap_message_id_H;
COM4004_EXT u8				ss_ap_message_id_L;
COM4004_EXT AB 				ss_ab;				//action backlight
COM4004_EXT SS_sense	ss_sensedata;	//data sync	,data rscent
COM4004_EXT	CS				ss_cs;
COM4004_EXT AR				ss_ar;				//action refresh���ָ��
COM4004_EXT DS				ss_ds;				//data sync���ָ��
COM4004_EXT CST				ss_cst[100];		//config stԤ��1���������6��st
COM4004_EXT u8				ss_cst_count;	//ʵ��config st������ss_cst_count��st��Ϣ
COM4004_EXT DES				ss_des;				//device status���ָ��
COM4004_EXT u8				ss_des_message_id_H;
COM4004_EXT u8				ss_des_message_id_L;
COM4004_EXT QE				ss_qe;				//qe���ָ��
COM4004_EXT u8				ss_qe_message_id_H;
COM4004_EXT u8				ss_qe_message_id_L;
COM4004_EXT RT				ss_rt;				//rtָ��
COM4004_EXT ALARM			ss_alarm;			//alarm���ָ��
COM4004_EXT CSHP			ss_cshp[20];	//config strategy hpstָ��,�ݶ���ౣ��20��strategy hpst����
COM4004_EXT u8				ss_csht_stid;	//config strategy hpspָ�����stid

COM4004_EXT u8				ss_di_message_id_H;//���յ�data/irָ�����message id������Ҫ�ظ�3��ָ��
COM4004_EXT u8				ss_di_message_id_L;

COM4004_EXT union  FLAG 		UART1Flag1_;
#define UART1Flag1 					UART1Flag1_._flag_byte
#define rev_success					UART1Flag1_._flag_bit.bit0	//���ڽ������ݷ���
#define ack_ap_rev_success	UART1Flag1_._flag_bit.bit1	//���ڻظ�action perform
#define ack_ap_exe_success	UART1Flag1_._flag_bit.bit2	//���ڻظ�action perform
#define ack_ap_exe_fail			UART1Flag1_._flag_bit.bit3	//���ڻظ�action perform
#define ack_ap_exe_done			UART1Flag1_._flag_bit.bit4	//���ڻظ�action perform
#define ack_ar							UART1Flag1_._flag_bit.bit5	//���ڻظ�action refreshˢ�����д�����
#define rev_ar1							UART1Flag1_._flag_bit.bit5	//���յ�action/refreshˢ��SS������ָ��
#define rev_ar2							UART1Flag1_._flag_bit.bit5	//���յ�action/refreshˢ��ST������ָ��
#define ack_ds							UART1Flag1_._flag_bit.bit6	//���ڻظ�data sync��ȡ���д�����
#define ack_ds2							UART1Flag1_._flag_bit.bit7	//���ڻظ�data sync���ͬ��ָ���ȡĳ��������

COM4004_EXT union  FLAG 		UART1Flag2_;
#define UART1Flag2 					UART1Flag2_._flag_byte
#define ack_ab							UART1Flag2_._flag_bit.bit0	//�ظ�action backlight
#define ack_dr							UART1Flag2_._flag_bit.bit1	//�ظ�data recentǿ��ˢ�´��������ݲ���ȡ
#define ack_dir_rev_success	UART1Flag2_._flag_bit.bit2	//�ظ�data ir�յ���ȡIR��������
#define ack_dir_reving			UART1Flag2_._flag_bit.bit3	//�ظ�data ir��ʼ��ʼ����ir����
#define ack_dir_rev_done		UART1Flag2_._flag_bit.bit4	//�ظ�data ir�ɹ�����IR����
#define ack_cs							UART1Flag2_._flag_bit.bit5	//�ظ�config ss(�������ͬ��ָ��),�ظ����ջ�ִ
#define ack_dl							UART1Flag2_._flag_bit.bit6	//�ظ�device list
#define ack_cm							UART1Flag2_._flag_bit.bit7	//�ظ�config mesh

COM4004_EXT union  FLAG 		UART1Flag3_;
#define UART1Flag3 					UART1Flag3_._flag_byte
#define ack_cst							UART1Flag3_._flag_bit.bit0	//�ظ�config st
#define ack_des							UART1Flag3_._flag_bit.bit1	//�ظ�device status
#define ack_des2						UART1Flag3_._flag_bit.bit2	//�ظ�device status���ָ��ظ�ĳһ��sep������ͨ������
#define ack_des3						UART1Flag3_._flag_bit.bit3	//�ظ�device status���ָ��ظ�ĳһ��sep��ĳ��ͨ������
#define ack_diss						UART1Flag3_._flag_bit.bit4	//�ظ�device info ss
#define ack_qe							UART1Flag3_._flag_bit.bit5	//�ظ�qe���ָ��
#define ack_alarm						UART1Flag3_._flag_bit.bit6	//�ظ�qe���ָ��
#define ack_cshp						UART1Flag3_._flag_bit.bit7	//�ظ�config strategy hpstָ��


COM4004_EXT union  FLAG 		UART1Flag4_;
#define rev_success2				UART1Flag4_._flag_bit.bit0
#define ack_csht						UART1Flag4_._flag_bit.bit1	//�ظ�config strategy htspָ��


COM4004_EXT union  FLAG 		UART1Flag5_;


COM4004_EXT union  FLAG 		UART1Flag6_;


COM4004_EXT union  FLAG 		UART1Flag7_;
#define UART1Flag7 					UART1Flag7_._flag_byte
#define ready_ss_post				UART1Flag7_._flag_bit.bit0	//����ss��������
#define ready_st_post				UART1Flag7_._flag_bit.bit1	//����st�������ݣ���uart2��st���ݴ洢��ss.sc[]����λ
#define ready_sc_post				UART1Flag7_._flag_bit.bit2	//����sc��������
#define ready_slc_post			UART1Flag7_._flag_bit.bit3	//����slc��������
#define ready_spc_post			UART1Flag7_._flag_bit.bit4	//����spc��������
#define rev_st_mal					UART1Flag7_._flag_bit.bit5	//���յ�ST�㱨����
#define rev_sc_mal					UART1Flag7_._flag_bit.bit6	//���յ�SC�㱨����
#define rev_slc_mal					UART1Flag7_._flag_bit.bit7	//���յ�SLC�㱨����

COM4004_EXT union  FLAG 		UART1Flag8_;
#define rev_spc_mal					UART1Flag8_._flag_bit.bit0	//���յ�SPC�㱨����
#define rev_heartbeat				UART1Flag8_._flag_bit.bit1	//���յ�Heartbeat��
#define rev_st_rt						UART1Flag8_._flag_bit.bit2	//���յ�st������⵽�˶�


COM4004_EXT u8 send_buf[100];
COM4004_EXT u8 rev_buf[MAX_BUF_LEN];
COM4004_EXT u8 ssp_buf[MAX_BUF_LEN];
COM4004_EXT u16 ssp_length;



COM4004_EXT u8 random(u8 xxx);
COM4004_EXT void UART2_Send_Data_Init(void);
COM4004_EXT void UART2_Send_Data_Start(void);
COM4004_EXT int test_cjson(void);
COM4004_EXT int mystrcmp(unsigned char *str1,const unsigned char *str2);
COM4004_EXT void rev_analyze(u8 *topic_buf,u8 *cjson_buf);
COM4004_EXT void mystrncat(char *desc,char *src,u32 len);
COM4004_EXT u8 ssp_parse(u8 *buf,u16 buf_len);
COM4004_EXT void analyze(void);
COM4004_EXT void success_receipt(void);
COM4004_EXT void send_message_without_payload(u8 fix1,u8 version,u8 message_id_h,u8 message_id_l,u8 ex_message_id);
COM4004_EXT void send_message_with_payload(u8 fix1,u8 version,u8 message_id_h,u8 message_id_l,u8 *payload);
COM4004_EXT void send_message(Txmessage *tx);
COM4004_EXT void clear_tx_buf(void);
void init_tx_message(u8 first_ch_byte,u8 version,u8 topic_len,char *topic,u8 message_id_H,u8 message_id_L,u8 ext_message_id,char *payload);
COM4004_EXT void init_send_Txmessage(_REPLY_ type);
COM4004_EXT void rev_heart_beat(u8 fix1);
COM4004_EXT void rev_action_perform(void);
COM4004_EXT void rev_qe(void);
COM4004_EXT void rev_device_status(void);
COM4004_EXT void deal_action_perform(u8 *buf);
COM4004_EXT void deal_config_ss(u8 *buf);
COM4004_EXT void deal_deepsyn_config_ss(u8 *buf1,u8 *buf2);
COM4004_EXT void deal_action_backlight(u8 *buf);
COM4004_EXT void deal_action_refresh(u8 *buf);
COM4004_EXT	void deal_data_sync(u8 *buf);
COM4004_EXT void deal_device_list(u8 *buf);
COM4004_EXT void deal_config_mesh(u8 *buf);
COM4004_EXT void deal_config_st(u8 *buf);
COM4004_EXT void deal_config_stragy_hpst(u8 *buf);
COM4004_EXT void deal_config_strategy_htsp(u8 *buf);//buf��topic����
COM4004_EXT void deal_device_status(u8 *buf);
COM4004_EXT void deal_qe(u8 *buf);
COM4004_EXT void dela_alarm(u8 *buf);

COM4004_EXT void send_config_ss(void);
COM4004_EXT void send_data_sync(u8 type);
COM4004_EXT void send_deepin_data_sync(void);
COM4004_EXT void send_device_info_sub(void);
COM4004_EXT void send_device_malfunction(void);


#endif


