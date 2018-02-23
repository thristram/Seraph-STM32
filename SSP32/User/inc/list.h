/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
#ifndef  __LIST_H__
#define  __LIST_H__

#include "sys.h"
#include "type.h"

/* 单向列表 */
typedef struct slnode_t{

	struct slnode_t *next;	

	u16 len;		/* data的数据长度 */
	char *data;

} slnode_t;

/* 单向列表 */
typedef struct irnode_t{

	struct irnode_t *next;
	
	u8 flag;		/* 0.还未开始发送 1.开始发送，并开始计时1s 2.发送完成，等待接收 */

	u8 time_cnt;		
	u16 msgid;	
	
	u16 len;		/* data的数据长度 */
	char *data;

} irnode_t;


typedef union{
	
	u8 byte;
	
	struct{
	        u8 sended:1;			//发送标志
	        u8 tcp_en:1;		
	        u8 post_devinfo_ss:1;
	        u8 recv_devinfo_list:1;
	        u8 recv_config_ss:1;
		u8 sicp_broadcast_finish:1;
	        u8 request_config_st:1;
	        u8 recv_config_st:1;

  	}bit;
	
} reSend_status_t;


/* 重传列表 */
/* 确认接收到则直接删除， 另定时2s发送一次，每次最多发送10包数据 */
/* 从列表尾部添加，从链表任意位置删除 */
typedef struct reSendNode_t{

	struct reSendNode_t *prev;
	struct reSendNode_t *next;

	u8 	len;		/* data的数据长度 */
	u8 	*data;

	u8	msgid;

} reSendNode_t;


typedef struct{

	struct reSendNode_t *prev;
	struct reSendNode_t *next;

	u8 	len;		/* data的数据长度 */
	u8 	*data;

	u8	msgid;

} reSend_t;




/* reSend重传列表的表头 */
extern reSendNode_t *reSendHead;

/* reSend重传列表的表尾 */
extern reSendNode_t *reSendLast;

/* reSend重传列表计时,100ms为单位*/
extern int reSendCount;



int addNodeToReSendLast(u8 *psave, u8 length);

int deleteNodeFromReSend(reSendNode_t *deleteNode);




extern u8 ssp_uart_mutex;
extern u8 sicp_uart_mutex;



/* uart发送单项队列SingleList的表头 */
extern slnode_t *uartTxSLHead;

/* uart发送单项队列SingleList的表尾 */
extern slnode_t *uartTxSLLast;

/* uart发送单项队列SingleList的表头 */
extern slnode_t *uart2TxSLHead;

/* uart发送单项队列SingleList的表尾 */
extern slnode_t *uart2TxSLLast;


/* IR发送单项队列SingleList的表头 */
extern irnode_t *IRTxSLHead;

/* IR发送单项队列SingleList的表尾 */
extern irnode_t *IRTxSLLast;




/* uart接收单项队列SingleList的表头 */
extern slnode_t *uartRxSLHead;

/* uart接收单项队列SingleList的表尾 */
extern slnode_t *uartRxSLLast;


int addNodeToUartTxSLLast(char *psave, int length);
int addNodeToUart2TxSLLast(char *psave, int length);
int addNodeToUartRxSLLast(char *psave, int length);
int addNodeTo_IRTxSLLast(char *psave, int length, u16 msgid);
int deleteNodeFrom_IRTxSLHead(void);
int deleteNodeFromUartTxSLHead(void);
int deleteNodeFromUart2TxSLHead(void);
int deleteNodeFromUartRxSLHead(void);
void clearUartTxSL(void);
void clearUartRxSL(void);



#endif

