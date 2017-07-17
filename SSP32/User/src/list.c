/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/

#include "includes.h"

/* uart发送单项队列SingleList的表头 */
slnode_t *uartTxSLHead = NULL;

/* uart发送单项队列SingleList的表尾 */
slnode_t *uartTxSLLast = NULL;
/* uart发送单项队列SingleList的表头 */
slnode_t *uart2TxSLHead = NULL;

/* uart发送单项队列SingleList的表尾 */
slnode_t *uart2TxSLLast = NULL;

/* uart接收单项队列SingleList的表头 */
slnode_t *uartRxSLHead = NULL;

/* uart接收单项队列SingleList的表尾 */
slnode_t *uartRxSLLast = NULL;

u8 mutex = 0;
u8 mutex2 = 0;
/*
	tx_event_flags_create(&uartTxSLEventGroup, "uart tx event group");	
	tx_event_flags_set(&uartTxSLEventGroup, uartTxSLEventFlag, TX_OR);
	tx_event_flags_get(&uartTxSLEventGroup, uartTxSLEventFlag, TX_AND_CLEAR, &uartTxSLActualFlag, 100*1000);

	
	tx_event_flags_create(&uartRxSLEventGroup, "uart rx event group");
	tx_event_flags_set(&uartRxSLEventGroup, uartRxSLEventFlag, TX_OR);
	tx_event_flags_get(&uartRxSLEventGroup, uartRxSLEventFlag, TX_AND_CLEAR, &uartRxSLActualFlag, 100*1000);


	TX_WAIT_FOREVER	
	TX_NO_WAIT
*/



/*   A D D   N O D E   T O   U A R T   T X   S   L   L A S T   */
/*-------------------------------------------------------------------------
    此函数与链表can1TxSLHead UartTxSLLast 直接绑定
    不能作为通用的函数
    函数不可冲入，因此互斥量需同时锁住链表和函数    
-------------------------------------------------------------------------*/
int addNodeToUartTxSLLast(char *psave, int length)
{
	int ret = -1;
	slnode_t * newNode;
	char *pdata;

	newNode = (slnode_t *)mymalloc(sizeof(slnode_t)); if(!newNode){ return -1;}
	/* 0A 0A */
	pdata = (char *)mymalloc(length+4); if(!pdata){myfree(newNode); return -1;}
	mutex = 1;
	if(newNode && pdata){
		newNode->next = NULL;
		newNode->len = (u16)length + 2;	/* 帧尾 0A 0A */
		newNode->hasWrite = 0;
		mymemcpy(pdata, psave, length);
		//因为传入的psave有分配内存，在这里需释放内存
		myfree(psave);
		*(pdata+length) = 0x0a;
		*(pdata+length+1)= 0x0a;
		newNode->data = pdata;
		mutex = 0;
		if(!uartTxSLLast){	/* 链表尾是否为空? */
			uartTxSLHead = newNode;
			uartTxSLLast = newNode;
			mutex = 0;
		}else{
			uartTxSLLast->next = newNode;	/* add node */
			uartTxSLLast = newNode;	/* new list end */
			mutex = 0;			
		}
		ret = 0;
	}	
	return ret;
}

int addNodeToUart2TxSLLast(char *psave, int length)
{
	int ret = -1;
	slnode_t * newNode;
	char *pdata;

	newNode = (slnode_t *)mymalloc(sizeof(slnode_t)); if(!newNode){ return -1;}
	/* 0A 0A */
	pdata = (char *)mymalloc(length); if(!pdata){myfree(newNode); return -1;}
	mutex2 = 1;
	if(newNode && pdata){
		newNode->next = NULL;
		newNode->len = (u16)length;	
		newNode->hasWrite = 0;
		mymemcpy(pdata, psave, length);
		//因为传入的psave有分配内存，在这里需释放内存
		myfree(psave);
		//*(pdata+length) = 0x0a;
		//*(pdata+length+1)= 0x0a;
		newNode->data = pdata;
		mutex2 = 0;
		if(!uart2TxSLLast){	/* 链表尾是否为空? */
			uart2TxSLHead = newNode;
			uart2TxSLLast = newNode;
			mutex2 = 0;
		}else{
			uart2TxSLLast->next = newNode;	/* add node */
			uart2TxSLLast = newNode;	/* new list end */
			mutex2 = 0;			
		}
		ret = 0;
	}	
	return ret;
}
/*   A D D   N O D E   T O   C A N 0   T X   S   L   L A S T   */
/*-------------------------------------------------------------------------
    此函数与链表can1TxSLHead can1TxSLLast 直接绑定
    不能作为通用的函数
    函数不可冲入，因此互斥量需同时锁住链表和函数      
-------------------------------------------------------------------------*/
int addNodeToUartRxSLLast(char *psave, int length)
{
	int ret = -1;
	slnode_t * newNode;
	char *pdata;
	newNode = (slnode_t *)mymalloc(sizeof(slnode_t)); if(!newNode){ return -1;}
	pdata = (char *)mymalloc(length+1); if(!pdata){myfree(newNode); return -1;}
	mutex = 1;
	if(newNode && pdata){
		newNode->next = NULL;
		newNode->len = (u16)length;
		newNode->hasWrite = 0;
		mymemcpy(pdata, psave, length);
		newNode->data = pdata;
		
		if(!uartRxSLLast){	/* 链表尾是否为空? */
			uartRxSLHead = newNode;
			uartRxSLLast = newNode;
			mutex = 0;
		}else{
			uartRxSLLast->next = newNode;	/* add node */
			uartRxSLLast = newNode;	/* new list end */
			mutex = 0;
		}
		
		ret = 0;
	}	



//newNode = (slnode_t *)mem_alloc(sizeof(slnode_t)); if(!newNode){ return -1;}
//if(newNode){
//	newNode->next = NULL;
//	newNode->len = (uint)length;
//	newNode->hasWrite = 0;
//	newNode->data = psave;
//
//	if(!uartRxSLLast){	/* 链表尾是否为空? */
//		uartRxSLHead = newNode;
//		uartRxSLLast = newNode;
//	}else{
//		uartRxSLLast->next = newNode;	/* add node */
//		uartRxSLLast = newNode; /* new list end */		
//	}
//	ret = 0;
//}	

return ret;
	
}


/*   D E L E T E   N O D E   F R O M   C A N 1   T X   S   L   H E A D   */
/*-------------------------------------------------------------------------
        从链表头删除节点
-------------------------------------------------------------------------*/
int deleteNodeFromUartTxSLHead(void)
{
	int ret = -1;
	slnode_t *newhead;

	if(uartTxSLHead){	/* 链表头不为空 */	
		if(!uartTxSLHead->next){	/* 链表头的next为空，即只有一个节点，链表头和链表尾都指向该节点 */
			if(uartTxSLHead->data) {myfree(uartTxSLHead->data);}
			myfree(uartTxSLHead);
			uartTxSLHead = NULL;
			uartTxSLLast = NULL;
		}else{	/* 链表头的next不为空，即有两个或两个以上的节点 */
			newhead = uartTxSLHead->next;
			if(uartTxSLHead->data) {myfree(uartTxSLHead->data);}
			myfree(uartTxSLHead);
			uartTxSLHead = newhead;			
		}	
		ret = 0;
	}
	return ret;	
}

int deleteNodeFromUart2TxSLHead(void)
{
	int ret = -1;
	slnode_t *newhead;

	if(uart2TxSLHead){	/* 链表头不为空 */	
		if(!uart2TxSLHead->next){	/* 链表头的next为空，即只有一个节点，链表头和链表尾都指向该节点 */
			if(uart2TxSLHead->data) {myfree(uart2TxSLHead->data);}
			myfree(uart2TxSLHead);
			uart2TxSLHead = NULL;
			uart2TxSLLast = NULL;
		}else{	/* 链表头的next不为空，即有两个或两个以上的节点 */
			newhead = uart2TxSLHead->next;
			if(uart2TxSLHead->data) {myfree(uart2TxSLHead->data);}
			myfree(uart2TxSLHead);
			uart2TxSLHead = newhead;			
		}	
		ret = 0;
	}
	return ret;	
}
/*   D E L E T E   N O D E   F R O M   C A N 0   T X   S   L   H E A D   */
/*-------------------------------------------------------------------------
   从链表头删除节点
-------------------------------------------------------------------------*/
int deleteNodeFromUartRxSLHead(void)
{
	int ret = -1;
	slnode_t *newhead;

	if(uartRxSLHead){	/* 链表头不为空 */	
		if(!uartRxSLHead->next){	/* 链表头的next为空，即只有一个节点，链表头和链表尾都指向该节点 */
			if(uartRxSLHead->data) {myfree(uartRxSLHead->data);}
			myfree(uartRxSLHead);
			uartRxSLHead = NULL;
			uartRxSLLast = NULL;
		}else{	/* 链表头的next不为空，即有两个或两个以上的节点 */
			newhead = uartRxSLHead->next;
			if(uartRxSLHead->data) {myfree(uartRxSLHead->data);}
			myfree(uartRxSLHead);
			uartRxSLHead = newhead;			
		}	
		ret = 0;
	}

//	if(uartRxSLHead){	/* 链表头不为空 */	
//		if(!uartRxSLHead->next){	/* 链表头的next为空，即只有一个节点，链表头和链表尾都指向该节点 */
//			mem_free(uartRxSLHead);
//			uartRxSLHead = NULL;
//			uartRxSLLast = NULL;
//		}else{	/* 链表头的next不为空，即有两个或两个以上的节点 */
//			newhead = uartRxSLHead->next;
//			mem_free(uartRxSLHead);
//			uartRxSLHead = newhead;			
//		}	
//		ret = 0;
//	}


	
	return ret;	
}



/*   C L E A R   C A N 1   T X   S   L   */
/*-------------------------------------------------------------------------
       使用过程如果出现错误，则清空链表 
-------------------------------------------------------------------------*/
void clearUartTxSL(void)
{
	slnode_t *pNext;
	
	while(uartTxSLHead){		
		pNext = uartTxSLHead->next;
		if(uartTxSLHead->data){myfree(uartTxSLHead->data);} 
		myfree(uartTxSLHead);
		uartTxSLHead = pNext;
	}
	uartTxSLLast = NULL;
}



/*   C L E A R   C A N 0   T X   S   L   */
/*-------------------------------------------------------------------------
    使用过程如果出现错误，则清空链表 
-------------------------------------------------------------------------*/
void clearUartRxSL(void)
{
	slnode_t *pNext;
	
	while(uartRxSLHead){
		pNext = uartRxSLHead->next;
		if(uartRxSLHead->data) {myfree(uartRxSLHead->data);}
		myfree(uartRxSLHead);
		uartRxSLHead = pNext;
	}
	uartRxSLLast = NULL;
}









