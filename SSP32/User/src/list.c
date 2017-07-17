/*-------------------------------------------------------------------------
    
-------------------------------------------------------------------------*/

#include "includes.h"

/* uart���͵������SingleList�ı�ͷ */
slnode_t *uartTxSLHead = NULL;

/* uart���͵������SingleList�ı�β */
slnode_t *uartTxSLLast = NULL;
/* uart���͵������SingleList�ı�ͷ */
slnode_t *uart2TxSLHead = NULL;

/* uart���͵������SingleList�ı�β */
slnode_t *uart2TxSLLast = NULL;

/* uart���յ������SingleList�ı�ͷ */
slnode_t *uartRxSLHead = NULL;

/* uart���յ������SingleList�ı�β */
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
    �˺���������can1TxSLHead UartTxSLLast ֱ�Ӱ�
    ������Ϊͨ�õĺ���
    �������ɳ��룬��˻�������ͬʱ��ס����ͺ���    
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
		newNode->len = (u16)length + 2;	/* ֡β 0A 0A */
		newNode->hasWrite = 0;
		mymemcpy(pdata, psave, length);
		//��Ϊ�����psave�з����ڴ棬���������ͷ��ڴ�
		myfree(psave);
		*(pdata+length) = 0x0a;
		*(pdata+length+1)= 0x0a;
		newNode->data = pdata;
		mutex = 0;
		if(!uartTxSLLast){	/* ����β�Ƿ�Ϊ��? */
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
		//��Ϊ�����psave�з����ڴ棬���������ͷ��ڴ�
		myfree(psave);
		//*(pdata+length) = 0x0a;
		//*(pdata+length+1)= 0x0a;
		newNode->data = pdata;
		mutex2 = 0;
		if(!uart2TxSLLast){	/* ����β�Ƿ�Ϊ��? */
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
    �˺���������can1TxSLHead can1TxSLLast ֱ�Ӱ�
    ������Ϊͨ�õĺ���
    �������ɳ��룬��˻�������ͬʱ��ס����ͺ���      
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
		
		if(!uartRxSLLast){	/* ����β�Ƿ�Ϊ��? */
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
//	if(!uartRxSLLast){	/* ����β�Ƿ�Ϊ��? */
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
        ������ͷɾ���ڵ�
-------------------------------------------------------------------------*/
int deleteNodeFromUartTxSLHead(void)
{
	int ret = -1;
	slnode_t *newhead;

	if(uartTxSLHead){	/* ����ͷ��Ϊ�� */	
		if(!uartTxSLHead->next){	/* ����ͷ��nextΪ�գ���ֻ��һ���ڵ㣬����ͷ������β��ָ��ýڵ� */
			if(uartTxSLHead->data) {myfree(uartTxSLHead->data);}
			myfree(uartTxSLHead);
			uartTxSLHead = NULL;
			uartTxSLLast = NULL;
		}else{	/* ����ͷ��next��Ϊ�գ������������������ϵĽڵ� */
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

	if(uart2TxSLHead){	/* ����ͷ��Ϊ�� */	
		if(!uart2TxSLHead->next){	/* ����ͷ��nextΪ�գ���ֻ��һ���ڵ㣬����ͷ������β��ָ��ýڵ� */
			if(uart2TxSLHead->data) {myfree(uart2TxSLHead->data);}
			myfree(uart2TxSLHead);
			uart2TxSLHead = NULL;
			uart2TxSLLast = NULL;
		}else{	/* ����ͷ��next��Ϊ�գ������������������ϵĽڵ� */
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
   ������ͷɾ���ڵ�
-------------------------------------------------------------------------*/
int deleteNodeFromUartRxSLHead(void)
{
	int ret = -1;
	slnode_t *newhead;

	if(uartRxSLHead){	/* ����ͷ��Ϊ�� */	
		if(!uartRxSLHead->next){	/* ����ͷ��nextΪ�գ���ֻ��һ���ڵ㣬����ͷ������β��ָ��ýڵ� */
			if(uartRxSLHead->data) {myfree(uartRxSLHead->data);}
			myfree(uartRxSLHead);
			uartRxSLHead = NULL;
			uartRxSLLast = NULL;
		}else{	/* ����ͷ��next��Ϊ�գ������������������ϵĽڵ� */
			newhead = uartRxSLHead->next;
			if(uartRxSLHead->data) {myfree(uartRxSLHead->data);}
			myfree(uartRxSLHead);
			uartRxSLHead = newhead;			
		}	
		ret = 0;
	}

//	if(uartRxSLHead){	/* ����ͷ��Ϊ�� */	
//		if(!uartRxSLHead->next){	/* ����ͷ��nextΪ�գ���ֻ��һ���ڵ㣬����ͷ������β��ָ��ýڵ� */
//			mem_free(uartRxSLHead);
//			uartRxSLHead = NULL;
//			uartRxSLLast = NULL;
//		}else{	/* ����ͷ��next��Ϊ�գ������������������ϵĽڵ� */
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
       ʹ�ù���������ִ������������ 
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
    ʹ�ù���������ִ������������ 
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









