/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
#ifndef  __LIST_H__
#define  __LIST_H__

#include "sys.h"
#include "type.h"

/* �����б� */
typedef struct slnode_t{

	struct slnode_t *next;	

	u16 len;		/* data�����ݳ��� */
	char *data;

} slnode_t;

/* �����б� */
typedef struct irnode_t{

	struct irnode_t *next;
	
	u8 flag;		/* 0.��δ��ʼ���� 1.��ʼ���ͣ�����ʼ��ʱ1s 2.������ɣ��ȴ����� */

	u8 time_cnt;		
	u16 msgid;	
	
	u16 len;		/* data�����ݳ��� */
	char *data;

} irnode_t;


typedef union{
	
	u8 byte;
	
	struct{
	        u8 sended:1;			//���ͱ�־
	        u8 tcp_en:1;		
	        u8 post_devinfo_ss:1;
	        u8 recv_devinfo_list:1;
	        u8 recv_config_ss:1;
		u8 sicp_broadcast_finish:1;
	        u8 request_config_st:1;
	        u8 recv_config_st:1;

  	}bit;
	
} reSend_status_t;


/* �ش��б� */
/* ȷ�Ͻ��յ���ֱ��ɾ���� ��ʱ2s����һ�Σ�ÿ����෢��10������ */
/* ���б�β����ӣ�����������λ��ɾ�� */
typedef struct reSendNode_t{

	struct reSendNode_t *prev;
	struct reSendNode_t *next;

	u8 	len;		/* data�����ݳ��� */
	u8 	*data;

	u8	msgid;

} reSendNode_t;


typedef struct{

	struct reSendNode_t *prev;
	struct reSendNode_t *next;

	u8 	len;		/* data�����ݳ��� */
	u8 	*data;

	u8	msgid;

} reSend_t;




/* reSend�ش��б�ı�ͷ */
extern reSendNode_t *reSendHead;

/* reSend�ش��б�ı�β */
extern reSendNode_t *reSendLast;

/* reSend�ش��б��ʱ,100msΪ��λ*/
extern int reSendCount;



int addNodeToReSendLast(u8 *psave, u8 length);

int deleteNodeFromReSend(reSendNode_t *deleteNode);




extern u8 ssp_uart_mutex;
extern u8 sicp_uart_mutex;



/* uart���͵������SingleList�ı�ͷ */
extern slnode_t *uartTxSLHead;

/* uart���͵������SingleList�ı�β */
extern slnode_t *uartTxSLLast;

/* uart���͵������SingleList�ı�ͷ */
extern slnode_t *uart2TxSLHead;

/* uart���͵������SingleList�ı�β */
extern slnode_t *uart2TxSLLast;


/* IR���͵������SingleList�ı�ͷ */
extern irnode_t *IRTxSLHead;

/* IR���͵������SingleList�ı�β */
extern irnode_t *IRTxSLLast;




/* uart���յ������SingleList�ı�ͷ */
extern slnode_t *uartRxSLHead;

/* uart���յ������SingleList�ı�β */
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

