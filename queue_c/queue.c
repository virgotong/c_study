/*队列是限制在两端进行插入操作和删除操作的线性表，允许进行存入操作的一端称为“队尾”，允许进行删除操作的一端称为“队头”。当线性表中没有元素时，称为“空队”。特点：先进先出（FIFO）*/

#include "queue.h"

void createQueue(PQUEUE Q, int maxsize)
{	
	Q->pBase = (int *)malloc(sizeof(int) * (maxsize));

	if(Q->pBase == NULL)
	{
		printf("Memeory allocation failure!\n");
		exit(EXIT_FAILURE);
	}

	Q->front = 0;
	Q->rear = 0;
	Q->maxsize = maxsize;	
}

void printQueue(PQUEUE Q)
{
	int i = Q->front;
	printf("Start print the stack elements...\n");
	while( i%Q->maxsize != Q->rear )
	{
		
		printf("rear: %d  value: %d  i: %d\n", Q->rear, Q->pBase[i], i%Q->maxsize);
		i++;		
	}
	printf("\n");
}

BOOL emptyQueue(PQUEUE Q)
{
	if( Q->front == Q->rear )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL fullQueue( PQUEUE Q)
{
	if(Q->front == (Q->rear+1) % (Q->maxsize)) //此时队列中实际元素个数是maxsize-1
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL enQueue(PQUEUE Q, int data)
{
	if(fullQueue(Q))
	{
		return FALSE;
	}
	else
	{
		Q->pBase[Q->rear] = data;
		Q->rear = (Q->rear + 1) % (Q->maxsize);
		return TRUE;
	}
}

BOOL deQueue(PQUEUE Q, int *data)
{
	if(emptyQueue(Q))
	{
		return FALSE;
	}
	else
	{
		*data = Q->pBase[Q->front];
		Q->front = (Q->front + 1) % (Q->maxsize);
		return TRUE;
	}
}

BOOL destoryQueue(PQUEUE Q)
{
	if(Q->pBase)
	{
		free(Q->pBase);
	}
	Q->pBase = NULL;
	Q->front = Q->rear = 0;
	return TRUE;
}


int main(int argc, char *argv[])
{
	int value;
	int i;
	QUEUE queue;
	PQUEUE lqueue = &queue;

	createQueue(lqueue,10);

	for(i = 0; i < 9; i++)
	{
		check(enQueue(lqueue, i+1));
	}

	printQueue(lqueue);

	for(i = 0; i < 9; i++)
	{
		deQueue(lqueue, &value);
		printf("%d\n", value);	
	}

	if(destoryQueue(lqueue))
	{
		printf("Destory queue success!\n");
	}
	
	return 0;
}

