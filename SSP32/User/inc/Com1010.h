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

#define Usart2_Rec_Len	100
#define Usart2_Send_Len	100

/***************message id����****************************
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
sc1~sc5�ֱ�Ϊ100~104

led-command:
st1~st20�ֱ�Ϊ110~119

alert-command:
st1~st20�ֱ�Ϊ120~139
sc1~sc5�ֱ�Ϊ140~144
malfuncrtion command:

Gateway	Mesh	ID	Broadcasting:
0x9E 158

*********************************************************/


typedef struct
{
	u8 frame_h1;
	u8 frame_h2;
	u8 message_id;
	u8 mesh_id_H;
	u8 mesh_id_L;
	u8 payload[50];
}SICP_Message;

typedef struct
{
	char deviceid[8];
	u8 message_id;
	u8 mesh_id_H;
	u8 mesh_id_L;
	u8 action_timeout;
	u8 action_cmd;
	u8 action_channel;
	u8 action_value;
	u8 action_ext;
	u8 action_ing;//�ȴ���1��ָ��ͱ�־
}ACTION;

typedef struct
{
	u8 signal;
	u8 connect;
	u8 phones;
	u8 host_meshid_H;
	u8 host_meshid_L;
}NET_Status;


/***************λ��������****************************/

COM1010_EXT union  FLAG 						UART2Flag1_;
#define UART2Flag1 									UART2Flag1_._flag_byte
#define usart2_rev_success					UART2Flag1_._flag_bit.bit0
#define ble_data_frame							UART2Flag1_._flag_bit.bit1//�յ�BLE����֡,֡ͷ0xEE 0xEE
#define ble_ctrl_frame							UART2Flag1_._flag_bit.bit2//�յ�BLE����֡��֡ͷ0xDD,0xDD
#define rev_action_done							UART2Flag1_._flag_bit.bit3//���յ�����ss_ap��sicp�ظ�
#define rev_action_done2						UART2Flag1_._flag_bit.bit4//���յ�qe��sicp�ظ�
#define send_dr_refresh_cmd_done		UART2Flag1_._flag_bit.bit5//ssp�յ�data/recent��sicp����ˢ�´������������

COM1010_EXT union  FLAG 						UART2Flag2_;
#define UART2Flag2 									UART2Flag2_._flag_byte
#define rev_20											UART2Flag2_._flag_bit.bit0//���յ�0x20ָ��
#define rev_06											UART2Flag2_._flag_bit.bit1//���յ�0x06ָ��
#define rev_B1											UART2Flag2_._flag_bit.bit2//���յ�0xB1ָ��
#define rev_B2											UART2Flag2_._flag_bit.bit3//���յ�0xB2ָ��
#define rev_B3											UART2Flag2_._flag_bit.bit4//���յ�0xB3ָ��
#define rev_B4											UART2Flag2_._flag_bit.bit5//���յ�0xB4ָ��


/***************ȫ�ֱ�������**************************/
COM1010_EXT u8  Usart2_Rece_Buf[Usart2_Rec_Len];
COM1010_EXT u8	Usart2_Rec_Cnt;
COM1010_EXT u8  USART2_Send_Buf[Usart2_Send_Len];		//���ͻ�����
COM1010_EXT u16 Usart2_Send_Length;
COM1010_EXT u16 Usart2_Send_Cnt;
COM1010_EXT u8  Usart2_Send_Done;//������ɱ�־
COM1010_EXT u8  sicp_buf[Usart2_Rec_Len];

COM1010_EXT u8  heartbeat_cnt;
COM1010_EXT u8  sicp_refr;
COM1010_EXT u8  broadcast_cnt;;

COM1010_EXT u8 action_type;
COM1010_EXT u8 action_timecnt;
COM1010_EXT u8 action_sendcnt;
COM1010_EXT ACTION action[10];
COM1010_EXT u8 rev_action_success_cnt;
COM1010_EXT u8 rev_action_fail_cnt;

COM1010_EXT ACTION action_qe;
COM1010_EXT NET_Status ns;
COM1010_EXT u8 	rev_mesh_id_H;
COM1010_EXT u8 	rev_mesh_id_L;

/**************��������*******************************/
COM1010_EXT u8 Check_Sum(u8 *buf,u8 length);
COM1010_EXT void rev_anaylze(void);
COM1010_EXT void clear_sicp_buf(void);
COM1010_EXT void sicp_send_message(SICP_Message *tx,u8 pay_len);
COM1010_EXT void rev_deal(void);
COM1010_EXT void sicp_send_heartbeart(void);
COM1010_EXT void sicp_receipt(u8 type,u8 send_message_id,u16 send_mesh_id);
COM1010_EXT void sicp_ble_cmd(u8 type,u8 send_message_id,u16 send_mesh_id);
COM1010_EXT void sicp_config_cmd(u8 send_message_id,u16 send_mesh_id);
COM1010_EXT void send_action_message(ACTION *ac);
COM1010_EXT void sicp_action_cmd(void);

COM1010_EXT void deal_device_info(u8 type);
COM1010_EXT void deal_rev_status(void);
COM1010_EXT void deal_mal_cmd(void);
COM1010_EXT void deal_sc_data_cmd(void);

COM1010_EXT void sicp_cmd_refresh(void);
COM1010_EXT void sicp_cmd_data(void);
COM1010_EXT void sicp_alert_cmd(void);
COM1010_EXT void sicp_led_cmd(void);
COM1010_EXT void sicp_qe_action_cmd(void);
COM1010_EXT void sicp_broadcast(void);


#endif



