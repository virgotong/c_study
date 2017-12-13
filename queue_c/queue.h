#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef unsigned char BOOL;

typedef struct queue
{
	int *pBase;
	int front,rear;
	int maxsize;
}QUEUE, *PQUEUE;

#define check( call ) 					\
	do{									\
		if(call){						\
			printf("入队成功！\n");		\
		}								\
		else{							\
			printf("入队失败！\n");		\
		}								\
	} while(0)						

/* create lqueue */
void createQueue(PQUEUE Q,int maxsize);

/*print queue*/
void printQueue( PQUEUE Q );

/*Determine whether the lqueue is empty*/
BOOL emptyQueue( PQUEUE Q );

/*Determine whether the lqueue is full*/
BOOL fullQueue( PQUEUE Q );

/*enLqueue*/
BOOL enQueue(PQUEUE Q, int data);

/*deLqueue*/
BOOL deQueue(PQUEUE Q, int *data);

/*destoryQueue*/
BOOL destoryQueue(PQUEUE Q);

#endif





