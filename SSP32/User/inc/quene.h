
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
	 queueptr front; //ָ��ͷ�� 
	 queueptr rear;  //ָ��β��
}linkqueue; 
 
QUENE_EXT void initqueue(linkqueue *q);
QUENE_EXT void enqueue(linkqueue *q,u8 *e);  //������Ԫ��e�������в���Ϊ��βԪ�� 
QUENE_EXT int lenqueue(linkqueue *q);  //����г��� 
QUENE_EXT u8 *dequeue(linkqueue *q,u8 *e);  //ɾ�������ж�ͷԪ�أ�����e������ֵ   
QUENE_EXT int queueempty(linkqueue *q); // �ж϶����Ƿ�Ϊ�� 


#endif

