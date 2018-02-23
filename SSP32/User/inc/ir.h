#ifndef __IR_H
#define __IR_H




#define IR_RAW_BIT_MAX	 		200
//50ms
#define IR_RAW_LEN_MAX	 		50000


#define IR_POSITION_LENGTH	 	3
#define IR_POSITION_COMMAND		5
#define IR_POSITION_TYPE			6
#define IR_POSITION_DATA			7

#define IR_AFTER_LENGTH_BYTES	 5


#define IR_RECV_LEN	 440
#define IR_SEND_LEN	 440 


typedef struct
{
	u8	addr;
	u8	rxflag;

	u16 	rxlen;		//总需要接收的字节数
	u16 	rxhas;		//已接收的字节数

	u16 	txlen;		//总需要发送的字节数
	u16 	txhas;		//已发送的字节数
	
	u8 	rxbuf[IR_RECV_LEN];
	u8 	txbuf[IR_SEND_LEN];

	u8	irbuf[IR_RECV_LEN];

	
}IR_frame_t;	//SICP的数据结构




extern  IR_frame_t IR_frame;






extern u8 use;







u8 ir_get_message_id(void);


void IR_init(void);



void IR_test(void);




void IR_Module_init(void);

void IR_send(void);

void IR_recv_handle(void);
		 				    
#endif
