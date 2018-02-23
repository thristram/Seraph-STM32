
//*************************************************************
#ifndef  QUENE_H
#define  QUENE_H
//*************************************************************

#ifndef  _QUENE_GLOBAL
#define  QUENE_EXT  extern 
#else
#define  QUENE_EXT 
#endif
#include "includes.h"
#include "sys.h"
#include "type.h"

typedef struct node  
{  
	 u8 *data;  
	 struct node *next;  
}node,*queueptr;

typedef struct  
{  
	 queueptr front; //指向头部 
	 queueptr rear;  //指向尾部
}linkqueue; 
 
QUENE_EXT void initqueue(linkqueue *q);
QUENE_EXT void enqueue(linkqueue *q,u8 *e);  //插入新元素e到队列中并成为队尾元素 
QUENE_EXT int lenqueue(linkqueue *q);  //求队列长度 
QUENE_EXT u8 *dequeue(linkqueue *q,u8 *e);  //删除队列中队头元素，并用e返回其值   
QUENE_EXT int queueempty(linkqueue *q); // 判断队列是否为空 


#endif

