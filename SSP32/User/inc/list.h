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
	u16 hasWrite;	/* data�Ѿ����͵����ݳ��� */
	char *data;

} slnode_t;

extern u8 mutex;
extern u8 mutex2;
/* uart���͵������SingleList�ı�ͷ */
extern slnode_t *uartTxSLHead;

/* uart���͵������SingleList�ı�β */
extern slnode_t *uartTxSLLast;

/* uart���͵������SingleList�ı�ͷ */
extern slnode_t *uart2TxSLHead;

/* uart���͵������SingleList�ı�β */
extern slnode_t *uart2TxSLLast;

/* uart���յ������SingleList�ı�ͷ */
extern slnode_t *uartRxSLHead;

/* uart���յ������SingleList�ı�β */
extern slnode_t *uartRxSLLast;


int addNodeToUartTxSLLast(char *psave, int length);
int addNodeToUart2TxSLLast(char *psave, int length);
int addNodeToUartRxSLLast(char *psave, int length);
int deleteNodeFromUartTxSLHead(void);
int deleteNodeFromUart2TxSLHead(void);
int deleteNodeFromUartRxSLHead(void);
void clearUartTxSL(void);
void clearUartRxSL(void);



#endif

