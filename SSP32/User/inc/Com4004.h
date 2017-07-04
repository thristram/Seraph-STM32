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
#define MAX              255            //Ëæ»úÊı×î´óÖµ
#define MIN              1              //Ëæ»úÊı×îĞ¡Öµ
#define MAX_BUF_LEN			 1024

//·¢ËÍÔö¼Ó¶ÓÁĞ£¬ĞÂÔö¶¨Òåsend_messageº¯ÊıµÄ·µ»Ø½á¹¹Ìå
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
	u8 r_length_len;//r_length[]Êı×éÊµ¼ÊÓĞÖµµÄ¸öÊı
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
	ACTION_PERPORM,	//action_performÖ¸ÁîÖ´ĞĞÍê³É
	ACTION_REFRESH,
	ACTION_BACKLIGHT,
	CONFIG_SS,		//Ö±½Ó»Ø¸´½ÓÊÕ»ØÖ´
	DATA_SYNC,
	POST_DATA_SYNC,//Ö÷¶¯ÍÆËÍ´«¸ĞÆ÷Êı¾İ
	DATA_RECENT,
	DATA_IR,			//»Ø¸´³É¹¦½ÓÊÕIRÊı¾İ
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
	SL_AP	sl_ap[10];	//Ôİ¶¨Ò»¸öSSÏÂÃæ×î¶àÒ»´ÎÊÕµ½10¸öSL actionÖ¸Áî
	u8 		sl_num;
	SP_AP	sp_ap[10];	//Ôİ¶¨Ò»¸öSSÏÂÃæ×î¶àÒ»´ÎÊÕµ½10¸öSP actionÖ¸Áî
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
	u8  motion;	//ÔË¶¯´«¸ĞÆ÷
	u8  presence;//ÈËÌå¼ì²â
	u8  bt;			//¹âÏßÇ¿¶È
	u8  CO_H;
	u8  CO_L;
	u8  CO2_H;
	u8  CO2_L;
	u8 	VOC;
	u8 	smoke;
	u8 	energy_diff;
	u8 	human_sensing;//1Î»ÓĞÈË£¬0ÎªÎŞÈË£¬11×óÓÒÁ½±ß¾ùÓĞÈË
}SS_sense;

typedef struct
{
	u16				meshid;
	char 			deviceid[8];
	char			firmware[3];
	int				HWTtest;
	char			model[3];//Éè±¸ĞÍºÅ
	char			coord[10];//Éè±¸ÔÚ·¿¼äµÄÎ»ÖÃ
	char			macwifi[17];
	int				MDID;//ÓÃÓÚ»ã±¨SLC/SPCµÄĞÅÏ¢£¬Ä£¿éID
	u8				posted;//Îª1Ê±±íÊ¾ÒÑ¾­ÓÉssÍÆËÍ¸øesh,Îª0±íÊ¾»¹Ã»ÓĞÍÆËÍ
	union 		FLAG status;//b0±íÊ¾Íø¹Ø½ÓÊÕµ½sicpµÄĞÄÌø°ü»Ø¸´
												//b1±íÊ¾ÊÕµ½sicp»Ø¸´channel×´Ì¬£¬Ö´ĞĞ³É¹¦
												//b2±íÊ¾ÊÕµ½sicp»Ø¸´action cmdÖ´ĞĞÊ§°Ü
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
	char			model[3];//Éè±¸ĞÍºÅ
	char			coord[10];//Éè±¸ÔÚ·¿¼äµÄÎ»ÖÃ
	char			macwifi[17];
	int				MDID;//ÓÃÓÚ»ã±¨SLC/SPCµÄĞÅÏ¢£¬Ä£¿éID
	u8				posted;//Îª1Ê±±íÊ¾ÒÑ¾­ÓÉssÍÆËÍ¸øesh,Îª0±íÊ¾»¹Ã»ÓĞÍÆË
	union 		FLAG status;//b0±íÊ¾Íø¹Ø½ÓÊÕµ½sicpµÄĞÄÌø°ü»Ø¸´
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
	char			model[3];//Éè±¸ĞÍºÅ
	char			coord[10];//Éè±¸ÔÚ·¿¼äµÄÎ»ÖÃ
	char			macwifi[17];
	SC_sense 	sense;
	int				Ndevice;//SCÏÂ¹ÒÓĞ¼¸¸öSLC/SPC
	SLC				slc[15];//Ôİ¶¨£¬Ò»¸öSCÏÂ×î¶à¹Ò15¸öslc
	SPC				spc[15];//Ôİ¶¨£¬Ò»¸öSCÏÂ×î¶à¹Ò15¸öspc
	u8				posted;//Îª1Ê±±íÊ¾ÒÑ¾­ÓÉssÍÆËÍ¸øesh,Îª0±íÊ¾»¹Ã»ÓĞÍÆË
	union 		FLAG status;//b0±íÊ¾Íø¹Ø½ÓÊÕµ½sicpµÄĞÄÌø°ü»Ø¸´
}SC;

typedef struct
{
	u16				meshid;
	char 			deviceid[8];
	char			firmware[3];
	int				HWTtest;
	char			model[3];//Éè±¸ĞÍºÅ
	char			coord[10];//Éè±¸ÔÚ·¿¼äµÄÎ»ÖÃ
	char			macwifi[17];
	ST_sense 	sense;
	u8				posted;//Îª1Ê±±íÊ¾ÒÑ¾­ÓÉssÍÆËÍ¸øesh,Îª0±íÊ¾»¹Ã»Ó
	union 		FLAG status;//b0±íÊ¾Íø¹Ø½ÓÊÕµ½sicpµÄĞÄÌø°ü»Ø¸´
												//b1±íÊ¾ÊÕµ½sicp»Ø¸´×´Ì¬£¬Ö´ĞĞ³É¹¦
												//b2±íÊ¾ÊÕµ½sicp»Ø¸´action cmdÖ´ĞĞÊ§°Ü
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
	char			model[2];//Éè±¸ĞÍºÅ
	char			coord[10];//Éè±¸ÔÚ·¿¼äµÄÎ»ÖÃ
	char			macwifi[17];
	CS			 	config;
	SS_sense 	sense;
	IR				ir;
	SC				sc[5];//Ôİ¶¨£¬Ò»¸öSSÏÂ×î¶à¹Ò5¸ösc
	ST				st[20];//Ôİ¶¨£¬Ò»¸öSSÏÂ×î¶à¹Ò20¸öst
	u8				posted;//Îª1Ê±±íÊ¾ÒÑ¾­ÓÉssÍÆËÍ¸øesh,Îª0±íÊ¾»¹Ã»ÓĞ
}SS;

typedef struct
{
	char		cond[100];
	u8			id;
	SL_AP		sl_ap[10];	//Ôİ¶¨Ò»¸öSSÏÂÃæ×î¶àÒ»´ÎÊÕµ½10¸öSL actionÖ¸Áî
	u8 			sl_num;
	SP_AP		sp_ap[10];	//Ôİ¶¨Ò»¸öSSÏÂÃæ×î¶àÒ»´ÎÊÕµ½10¸öSP actionÖ¸Áî
	u8 			sp_num;
}CSHP;	//config strategy hpst

typedef struct
{
	int				type;//typeÎª1Ê±±íÊ¾ÅäÖÃÀàĞÍÎª°´¼ü£¬Îª2Ê±ÎªÊÖÊÆ
	char			target_id[10];
	int				key;
	char			cond[3];
	int 			mdid;
	int 			ch;
	char			action[2];
	int 			value;
	u16				meshid;//´ËÎªÒªÅäÖÃµÄstµÄmeshid,Ğè×Ô¼º²éÕÒËùÓĞstµÄdeviceIdÀ´µÃµ½
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
		u8 		isPX;//½ÓÊÕµ½µÄÖ¸ÁîÊÇPX½Ó½ü´«¸Ğ
		int 	value;
	}px;
	struct	EG
	{
		u8 	isEG;	//½ÓÊÕµ½µÄÖ¸ÁîÊÇEGÓÃµçÁ¿
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
		u8		isGT;//½ÓÊÕµ½µÄÖ¸ÁîÊÇGTÊÖÊÆÊ¶±ğ
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


COM4004_EXT u8  DMA_Rece_Buf[DMA_Rec_Len];	   //DMA½ÓÊÕ´®¿ÚÊı¾İ»º³åÇø
COM4004_EXT u16 Usart1_Rec_Cnt;             	//±¾Ö¡Êı¾İ³¤¶È	
COM4004_EXT u8  USART1_Send_Buf[Send_Len];		//·¢ËÍ»º³åÇø
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
COM4004_EXT AR				ss_ar;				//action refreshÉî¶ÈÖ¸Áî
COM4004_EXT DS				ss_ds;				//data syncÉî¶ÈÖ¸Áî
COM4004_EXT CST				ss_cst[100];		//config stÔ¤Áô1´Î×î¶àÅäÖÃ6Ìõst
COM4004_EXT u8				ss_cst_count;	//Êµ¼Êconfig stÅäÖÃÁËss_cst_countÌõstĞÅÏ¢
COM4004_EXT DES				ss_des;				//device statusÉî¶ÈÖ¸Áî
COM4004_EXT u8				ss_des_message_id_H;
COM4004_EXT u8				ss_des_message_id_L;
COM4004_EXT QE				ss_qe;				//qeÉî¶ÈÖ¸Áî
COM4004_EXT u8				ss_qe_message_id_H;
COM4004_EXT u8				ss_qe_message_id_L;
COM4004_EXT RT				ss_rt;				//rtÖ¸Áî
COM4004_EXT ALARM			ss_alarm;			//alarmÉî¶ÈÖ¸Áî
COM4004_EXT CSHP			ss_cshp[20];	//config strategy hpstÖ¸Áî,Ôİ¶¨×î¶à±£´æ20Ìõstrategy hpstÃüÁî
COM4004_EXT u8				ss_csht_stid;	//config strategy hpspÖ¸Áî£¬±£´æstid

COM4004_EXT u8				ss_di_message_id_H;//½ÓÊÕµ½data/irÖ¸Áî£¬±£´æmessage id£¬ÒòĞèÒª»Ø¸´3ÌõÖ¸Áî
COM4004_EXT u8				ss_di_message_id_L;

COM4004_EXT union  FLAG 		UART1Flag1_;
#define UART1Flag1 					UART1Flag1_._flag_byte
#define rev_success					UART1Flag1_._flag_bit.bit0	//ÓÃÓÚ½ÓÊÕÊı¾İ·ÖÎö
#define ack_ap_rev_success	UART1Flag1_._flag_bit.bit1	//ÓÃÓÚ»Ø¸´action perform
#define ack_ap_exe_success	UART1Flag1_._flag_bit.bit2	//ÓÃÓÚ»Ø¸´action perform
#define ack_ap_exe_fail			UART1Flag1_._flag_bit.bit3	//ÓÃÓÚ»Ø¸´action perform
#define ack_ap_exe_done			UART1Flag1_._flag_bit.bit4	//ÓÃÓÚ»Ø¸´action perform
#define ack_ar							UART1Flag1_._flag_bit.bit5	//ÓÃÓÚ»Ø¸´action refreshË¢ĞÂËùÓĞ´«¸ĞÆ÷
#define rev_ar1							UART1Flag1_._flag_bit.bit5	//½ÓÊÕµ½action/refreshË¢ĞÂSS´«¸ĞÆ÷Ö¸Áî
#define rev_ar2							UART1Flag1_._flag_bit.bit5	//½ÓÊÕµ½action/refreshË¢ĞÂST´«¸ĞÆ÷Ö¸Áî
#define ack_ds							UART1Flag1_._flag_bit.bit6	//ÓÃÓÚ»Ø¸´data sync»ñÈ¡ËùÓĞ´«¸ĞÆ÷
#define ack_ds2							UART1Flag1_._flag_bit.bit7	//ÓÃÓÚ»Ø¸´data syncÉî¶ÈÍ¬²½Ö¸Áî»ñÈ¡Ä³¸ö´«¸ĞÆ÷

COM4004_EXT union  FLAG 		UART1Flag2_;
#define UART1Flag2 					UART1Flag2_._flag_byte
#define ack_ab							UART1Flag2_._flag_bit.bit0	//»Ø¸´action backlight
#define ack_dr							UART1Flag2_._flag_bit.bit1	//»Ø¸´data recentÇ¿ÖÆË¢ĞÂ´«¸ĞÆ÷Êı¾İ²¢»ñÈ¡
#define ack_dir_rev_success	UART1Flag2_._flag_bit.bit2	//»Ø¸´data irÊÕµ½»ñÈ¡IRÊı¾İÇëÇó
#define ack_dir_reving			UART1Flag2_._flag_bit.bit3	//»Ø¸´data ir¿ªÊ¼¿ªÊ¼½ÓÊÕirÊı¾İ
#define ack_dir_rev_done		UART1Flag2_._flag_bit.bit4	//»Ø¸´data ir³É¹¦½ÓÊÕIRÊı¾İ
#define ack_cs							UART1Flag2_._flag_bit.bit5	//»Ø¸´config ss(°üº¬Éî¶ÈÍ¬²½Ö¸Áî),»Ø¸´½ÓÊÕ»ØÖ´
#define ack_dl							UART1Flag2_._flag_bit.bit6	//»Ø¸´device list
#define ack_cm							UART1Flag2_._flag_bit.bit7	//»Ø¸´config mesh

COM4004_EXT union  FLAG 		UART1Flag3_;
#define UART1Flag3 					UART1Flag3_._flag_byte
#define ack_cst							UART1Flag3_._flag_bit.bit0	//»Ø¸´config st
#define ack_des							UART1Flag3_._flag_bit.bit1	//»Ø¸´device status
#define ack_des2						UART1Flag3_._flag_bit.bit2	//»Ø¸´device statusÉî¶ÈÖ¸Áî£¬»Ø¸´Ä³Ò»¸ösepÏÂËùÓĞÍ¨µÀÊı¾İ
#define ack_des3						UART1Flag3_._flag_bit.bit3	//»Ø¸´device statusÉî¶ÈÖ¸Áî£¬»Ø¸´Ä³Ò»¸ösepÏÂÄ³¸öÍ¨µÀÊı¾İ
#define ack_diss						UART1Flag3_._flag_bit.bit4	//»Ø¸´device info ss
#define ack_qe							UART1Flag3_._flag_bit.bit5	//»Ø¸´qeÉî¶ÈÖ¸Áî
#define ack_alarm						UART1Flag3_._flag_bit.bit6	//»Ø¸´qeÉî¶ÈÖ¸Áî
#define ack_cshp						UART1Flag3_._flag_bit.bit7	//»Ø¸´config strategy hpstÖ¸Áî


COM4004_EXT union  FLAG 		UART1Flag4_;
#define rev_success2				UART1Flag4_._flag_bit.bit0
#define ack_csht						UART1Flag4_._flag_bit.bit1	//»Ø¸´config strategy htspÖ¸Áî


COM4004_EXT union  FLAG 		UART1Flag5_;


COM4004_EXT union  FLAG 		UART1Flag6_;


COM4004_EXT union  FLAG 		UART1Flag7_;
#define UART1Flag7 					UART1Flag7_._flag_byte
#define ready_ss_post				UART1Flag7_._flag_bit.bit0	//ÍÆËÍss×ÔÉíÊı¾İ
#define ready_st_post				UART1Flag7_._flag_bit.bit1	//ÍÆËÍst×ÔÉíÊı¾İ£¬ÔÚuart2½«stÊı¾İ´æ´¢ÔÚss.sc[]ºóÖÃÎ»
#define ready_sc_post				UART1Flag7_._flag_bit.bit2	//ÍÆËÍsc×ÔÉíÊı¾İ
#define ready_slc_post			UART1Flag7_._flag_bit.bit3	//ÍÆËÍslc×ÔÉíÊı¾İ
#define ready_spc_post			UART1Flag7_._flag_bit.bit4	//ÍÆËÍspc×ÔÉíÊı¾İ
#define rev_st_mal					UART1Flag7_._flag_bit.bit5	//½ÓÊÕµ½ST»ã±¨¹ÊÕÏ
#define rev_sc_mal					UART1Flag7_._flag_bit.bit6	//½ÓÊÕµ½SC»ã±¨¹ÊÕÏ
#define rev_slc_mal					UART1Flag7_._flag_bit.bit7	//½ÓÊÕµ½SLC»ã±¨¹ÊÕÏ

COM4004_EXT union  FLAG 		UART1Flag8_;
#define rev_spc_mal					UART1Flag8_._flag_bit.bit0	//½ÓÊÕµ½SPC»ã±¨¹ÊÕÏ
#define rev_heartbeat				UART1Flag8_._flag_bit.bit1	//½ÓÊÕµ½Heartbeat°ü
#define rev_st_rt						UART1Flag8_._flag_bit.bit2	//½ÓÊÕµ½st·¢³ö¼ì²âµ½ÔË¶¯


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
COM4004_EXT void deal_config_strategy_htsp(u8 *buf);//bufÊÇtopicÄÚÈİ
COM4004_EXT void deal_device_status(u8 *buf);
COM4004_EXT void deal_qe(u8 *buf);
COM4004_EXT void dela_alarm(u8 *buf);

COM4004_EXT void send_config_ss(void);
COM4004_EXT void send_data_sync(u8 type);
COM4004_EXT void send_deepin_data_sync(void);
COM4004_EXT void send_device_info_sub(void);
COM4004_EXT void send_device_malfunction(void);


#endif


