
#define _QUENE_GLOBAL
#include "includes.h"
#include "stdlib.h"

void initqueue(linkqueue *q)  //初始化操作，建立一个空队列   
{  
	 q->front=q->rear=(queueptr)mymalloc(sizeof(node));  
	 if(!(q->front))  
			 return;  
	 q->front->next=0;  
}

void enqueue(linkqueue *q,u8 *e)  //插入新元素e到队列中并成为队尾元素   
{  
	 queueptr p;  
	 p=(queueptr)mymalloc(1000);  //暂定node结点(字符串长度+指针等)不超过1000
	 if(!p)	return;  
	 mymemcpy(p->data,e,strlen(e));
	 myfree(e);
	 *(p->data) = 0x0A;	//末尾添加0x0A 0x0A
	 *(p->data + 1) = 0x0A;
	 p->next=0;  
	 q->rear->next=p;  
	 q->rear=p;  
}  
 
void destroyqueue(linkqueue *q)  //若队列存在，则销毁它   
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

int lenqueue(linkqueue *q)  //求队列长度   
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
u8 *dequeue(linkqueue *q,u8 *e)  //删除队列中队头元素，并用e返回其值   
{  
	 node *p;  
	 if(q->front==q->rear)  
			 return -1;//队列为空  
	 p=q->front->next;  
	 mymemcpy(e,p->data,strlen(p->data));  
	 q->front->next=p->next;  
	 if(q->rear==p)  
			 q->rear=q->front;  
	 myfree(p);  
	 return e;  
}


int queueempty(linkqueue *q) // 判断队列是否为空   
{  
	 if(q->front==q->rear)  
			 return 1;  
	 else  
			 return 0;  
}  