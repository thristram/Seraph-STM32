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



/* IR发送单项队列SingleList的表头 */
irnode_t *IRTxSLHead = NULL;

/* IR发送单项队列SingleList的表尾 */
irnode_t *IRTxSLLast = NULL;





/* uart接收单项队列SingleList的表头 */
slnode_t *uartRxSLHead = NULL;

/* uart接收单项队列SingleList的表尾 */
slnode_t *uartRxSLLast = NULL;




/* reSend重传列表的表头 */
reSendNode_t *reSendHead = NULL;

/* reSend重传列表的表尾 */
reSendNode_t *reSendLast = NULL;

int reSendCount = 0;




u8 ssp_uart_mutex = 0;
u8 sicp_uart_mutex = 0;


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


/*-------------------------------------------------------------------------
	重传列表 增加节点 
-------------------------------------------------------------------------*/
int addNodeToReSendLast(u8 *psave, u8 length)
{
	int ret = -1;
	reSendNode_t * newNode;
	u8 *pdata;

	newNode = (reSendNode_t *)mymalloc(sizeof(reSendNode_t)); if(!newNode){ return -1;}

	pdata = (u8 *)mymalloc(length + 1); if(!pdata){myfree(newNode); return -1;}

	if(psave){
		
		newNode->prev = NULL;
		newNode->next = NULL;

		newNode->len = length ;	

		mymemcpy(pdata, psave, length);		
		newNode->data = pdata;
		
		newNode->msgid = *(psave + 2);

		if(!reSendLast){				/* 链表尾是否为空? */
			reSendHead = newNode;
			reSendLast = newNode;
			reSendCount = 0;
		}else{
			reSendLast->next = newNode;	/* add node */
			newNode->prev = reSendLast;
			reSendLast = newNode;		/* new list end */
		}
		ret = 0;
		
	}	
	
	return ret;
	
}


/*-------------------------------------------------------------------------
        从链表任意位置删除节点
-------------------------------------------------------------------------*/
int deleteNodeFromReSend(reSendNode_t *deleteNode)
{
	reSendNode_t *temp;

	if(deleteNode->prev){			//不是列表头
		
		temp = deleteNode->next;

		if(temp){
			
			(deleteNode->prev)->next = temp;
			temp->prev = deleteNode->prev;
			
		}else{					//deleteNode是列表尾

			reSendLast = deleteNode->prev;
			reSendLast->next = NULL;
		}
		
	}else{						//deleteNode为列表头

		reSendHead = deleteNode->next;
		
		if(reSendHead){
			
			reSendHead->prev = NULL;
			
		}else{					//deleteNode也是列表尾
			reSendLast = NULL;
		}
	}


	if(deleteNode->data) {myfree(deleteNode->data);}
	myfree(deleteNode);	

	return 0;	
	
}











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

	ssp_uart_mutex = 1;
	if(newNode && pdata){
		newNode->next = NULL;
		newNode->len = (u16)length + 2;	/* 帧尾 0A 0A */
		mymemcpy(pdata, psave, length);

		*(pdata+length) = 0x0a;
		*(pdata+length+1)= 0x0a;
		newNode->data = pdata;

		if(!uartTxSLLast){	/* 链表尾是否为空? */
			uartTxSLHead = newNode;
			uartTxSLLast = newNode;
		}else{
			uartTxSLLast->next = newNode;	/* add node */
			uartTxSLLast = newNode;	/* new list end */
		}
		ret = 0;
	}	
	ssp_uart_mutex = 0;
	
	return ret;
	
}




/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
int addNodeToUart2TxSLLast(char *psave, int length)
{
	int ret = -1;
	slnode_t * newNode;
	char *pdata;

	newNode = (slnode_t *)mymalloc(sizeof(slnode_t)); if(!newNode){ return -1;}
	pdata = (char *)mymalloc(length); if(!pdata){myfree(newNode); return -1;}

	sicp_uart_mutex = 1;
	if(psave){
		newNode->next = NULL;
		newNode->len = (u16)length;	
		mymemcpy(pdata, psave, length);

		newNode->data = pdata;
		if(!uart2TxSLLast){	/* 链表尾是否为空? */
			uart2TxSLHead = newNode;
			uart2TxSLLast = newNode;
		}else{
			uart2TxSLLast->next = newNode;	/* add node */
			uart2TxSLLast = newNode;	/* new list end */
		}
		ret = 0;
	}	
	sicp_uart_mutex = 0;

	return ret;
	
}



/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
int addNodeTo_IRTxSLLast(char *psave, int length, u16 msgid)
{
	int ret = -1;
	irnode_t * newNode;
	char *pdata;

	newNode = (irnode_t *)mymalloc(sizeof(irnode_t)); if(!newNode){ return -1;}
	pdata = (char *)mymalloc(length); if(!pdata){myfree(newNode); return -1;}
//	newNode = (irnode_t *)malloc(sizeof(irnode_t)); if(!newNode){ return -1;}
//	pdata = (char *)malloc(length); if(!pdata){myfree(newNode); return -1;}

	if(psave){
		
		newNode->next = NULL;
		newNode->len = (u16)length;
		newNode->msgid = msgid;
		newNode->flag = 0;
		newNode->time_cnt = 0;
		mymemcpy(pdata, psave, length);

		newNode->data = pdata;
		if(!IRTxSLLast){	/* 链表尾是否为空? */
			IRTxSLHead = newNode;
			IRTxSLLast = newNode;
		}else{
			IRTxSLLast->next = newNode;	/* add node */
			IRTxSLLast = newNode;	/* new list end */
		}
		ret = 0;
	}	

	return ret;
	
}

/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/
int deleteNodeFrom_IRTxSLHead(void)
{
	int ret = -1;
	irnode_t *newhead;

	if(IRTxSLHead){	/* 链表头不为空 */	
		if(!IRTxSLHead->next){	/* 链表头的next为空，即只有一个节点，链表头和链表尾都指向该节点 */
			if(IRTxSLHead->data) {myfree(IRTxSLHead->data);}
			myfree(IRTxSLHead);
			IRTxSLHead = NULL;
			IRTxSLLast = NULL;
		}else{	/* 链表头的next不为空，即有两个或两个以上的节点 */
			newhead = IRTxSLHead->next;
			if(IRTxSLHead->data) {myfree(IRTxSLHead->data);}
			myfree(IRTxSLHead);
			IRTxSLHead = newhead;			
		}	
		ret = 0;
	}
	return ret;	
}





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

//	newNode = (slnode_t *)mymalloc(sizeof(slnode_t)); if(!newNode){ return -1;}
//	newNode = (slnode_t *)malloc(sizeof(slnode_t)); if(!newNode){ return -1;}

	pdata = (char *)mymalloc(length+1); if(!pdata){myfree(newNode); return -1;}
	if(newNode && pdata){
		newNode->next = NULL;
		newNode->len = (u16)length;
		mymemcpy(pdata, psave, length);
		newNode->data = pdata;
		
		if(!uartRxSLLast){	/* 链表尾是否为空? */
			uartRxSLHead = newNode;
			uartRxSLLast = newNode;
		}else{
			uartRxSLLast->next = newNode;	/* add node */
			uartRxSLLast = newNode;	/* new list end */
		}
		
		ret = 0;
	}	



//newNode = (slnode_t *)mem_alloc(sizeof(slnode_t)); if(!newNode){ return -1;}
//if(newNode){
//	newNode->next = NULL;
//	newNode->len = (uint)length;
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









