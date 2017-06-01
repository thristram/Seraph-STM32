
#define _QUENE_GLOBAL
#include "includes.h"
#include "stdlib.h"

void initqueue(linkqueue *q)  //��ʼ������������һ���ն���   
{  
	 q->front=q->rear=(queueptr)mymalloc(sizeof(node));  
	 if(!(q->front))  
			 return;  
	 q->front->next=0;  
}

void enqueue(linkqueue *q,u8 *e)  //������Ԫ��e�������в���Ϊ��βԪ��   
{  
	 queueptr p;  
	 p=(queueptr)mymalloc(1000);  //�ݶ�node���(�ַ�������+ָ���)������1000
	 if(!p)	return;  
	 mymemcpy(p->data,e,strlen(e));
	 myfree(e);
	 *(p->data) = 0x0A;	//ĩβ���0x0A 0x0A
	 *(p->data + 1) = 0x0A;
	 p->next=0;  
	 q->rear->next=p;  
	 q->rear=p;  
}  
 
void destroyqueue(linkqueue *q)  //�����д��ڣ���������   
{  
	 if(!q)  
	 {  
			 return;  
	 }  
	 while(q->front)  
	 {  
			 q->rear=q->front->next;  
			 myfree((q->front));  
			 q->front=q->rear;  
			 if(!q->rear)  
					 myfree(q->rear);  
	 }  
	 myfree(q->front);  
}  

int lenqueue(linkqueue *q)  //����г���   
{  
	 node *r=q->front->next;  
	 int s=0;  
	 while(r)  
	 {  
			 r=r->next;  
			 s++;  
	 }  
	return s;  
}  
u8 *dequeue(linkqueue *q,u8 *e)  //ɾ�������ж�ͷԪ�أ�����e������ֵ   
{  
	 node *p;  
	 if(q->front==q->rear)  
			 return -1;//����Ϊ��  
	 p=q->front->next;  
	 mymemcpy(e,p->data,strlen(p->data));  
	 q->front->next=p->next;  
	 if(q->rear==p)  
			 q->rear=q->front;  
	 myfree(p);  
	 return e;  
}


int queueempty(linkqueue *q) // �ж϶����Ƿ�Ϊ��   
{  
	 if(q->front==q->rear)  
			 return 1;  
	 else  
			 return 0;  
}  