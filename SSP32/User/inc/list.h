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
	u16 hasWrite;	/* data已经发送的数据长度 */
	char *data;

} slnode_t;

extern u8 mutex;

/* uart发送单项队列SingleList的表头 */
extern slnode_t *uartTxSLHead;

/* uart发送单项队列SingleList的表尾 */
extern slnode_t *uartTxSLLast;


/* uart接收单项队列SingleList的表头 */
extern slnode_t *uartRxSLHead;

/* uart接收单项队列SingleList的表尾 */
extern slnode_t *uartRxSLLast;


int addNodeToUartTxSLLast(char *psave, int length);
int addNodeToUartRxSLLast(char *psave, int length);
int deleteNodeFromUartTxSLHead(void);
int deleteNodeFromUartRxSLHead(void);
void clearUartTxSL(void);
void clearUartRxSL(void);



#endif

