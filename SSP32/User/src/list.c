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



/* IR���͵������SingleList�ı�ͷ */
irnode_t *IRTxSLHead = NULL;

/* IR���͵������SingleList�ı�β */
irnode_t *IRTxSLLast = NULL;





/* uart���յ������SingleList�ı�ͷ */
slnode_t *uartRxSLHead = NULL;

/* uart���յ������SingleList�ı�β */
slnode_t *uartRxSLLast = NULL;




/* reSend�ش��б�ı�ͷ */
reSendNode_t *reSendHead = NULL;

/* reSend�ش��б�ı�β */
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
	�ش��б� ���ӽڵ� 
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

		if(!reSendLast){				/* ����β�Ƿ�Ϊ��? */
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
        ����������λ��ɾ���ڵ�
-------------------------------------------------------------------------*/
int deleteNodeFromReSend(reSendNode_t *deleteNode)
{
	reSendNode_t *temp;

	if(deleteNode->prev){			//�����б�ͷ
		
		temp = deleteNode->next;

		if(temp){
			
			(deleteNode->prev)->next = temp;
			temp->prev = deleteNode->prev;
			
		}else{					//deleteNode���б�β

			reSendLast = deleteNode->prev;
			reSendLast->next = NULL;
		}
		
	}else{						//deleteNodeΪ�б�ͷ

		reSendHead = deleteNode->next;
		
		if(reSendHead){
			
			reSendHead->prev = NULL;
			
		}else{					//deleteNodeҲ���б�β
			reSendLast = NULL;
		}
	}


	if(deleteNode->data) {myfree(deleteNode->data);}
	myfree(deleteNode);	

	return 0;	
	
}











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

	ssp_uart_mutex = 1;
	if(newNode && pdata){
		newNode->next = NULL;
		newNode->len = (u16)length + 2;	/* ֡β 0A 0A */
		mymemcpy(pdata, psave, length);

		*(pdata+length) = 0x0a;
		*(pdata+length+1)= 0x0a;
		newNode->data = pdata;

		if(!uartTxSLLast){	/* ����β�Ƿ�Ϊ��? */
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
		if(!uart2TxSLLast){	/* ����β�Ƿ�Ϊ��? */
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
		if(!IRTxSLLast){	/* ����β�Ƿ�Ϊ��? */
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

	if(IRTxSLHead){	/* ����ͷ��Ϊ�� */	
		if(!IRTxSLHead->next){	/* ����ͷ��nextΪ�գ���ֻ��һ���ڵ㣬����ͷ������β��ָ��ýڵ� */
			if(IRTxSLHead->data) {myfree(IRTxSLHead->data);}
			myfree(IRTxSLHead);
			IRTxSLHead = NULL;
			IRTxSLLast = NULL;
		}else{	/* ����ͷ��next��Ϊ�գ������������������ϵĽڵ� */
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
    �˺���������can1TxSLHead can1TxSLLast ֱ�Ӱ�
    ������Ϊͨ�õĺ���
    �������ɳ��룬��˻�������ͬʱ��ס����ͺ���      
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
		
		if(!uartRxSLLast){	/* ����β�Ƿ�Ϊ��? */
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









