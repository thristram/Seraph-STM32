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



/* uart���͵������SingleList�ı�ͷ */
extern slnode_t *uartTxSLHead;

/* uart���͵������SingleList�ı�β */
extern slnode_t *uartTxSLLast;


/* uart���յ������SingleList�ı�ͷ */
extern slnode_t *uartRxSLHead;

/* uart���յ������SingleList�ı�β */
extern slnode_t *uartRxSLLast;


int addNodeToUartTxSLLast(char *psave, int length);
int addNodeToUartRxSLLast(char *psave, int length);
int deleteNodeFromUartTxSLHead(void);
int deleteNodeFromUartRxSLHead(void);
void clearUartTxSL(void);
void clearUartRxSL(void);



#endif
