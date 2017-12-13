#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define MAXSIZE 100

typedef struct
{
	int *base;
	int *top;
	int size;
}Stack, *pStack;

int initStack( pStack s)
{
	s->top = (int *)malloc(sizeof(Stack) * MAXSIZE);
	if(s->top == NULL){
		printf("Memeory allocation failure!\n");
		exit(EXIT_FAILURE);
	}
	s->base = s->top;
	s->size = MAXSIZE;

	return 0;
}

int addStack(pStack s, int val)
{
	*s->top++ = val;

	return 0;
}

int printStack( pStack s)
{
	printf("%d %d\n", *(--s->top), s->size);

	return 0;
}

int main(int argc, char *argv[])
{
	Stack a;
	pStack stack = &a;
	initStack( stack );
	addStack(stack, 10);

	printStack( stack );
	printf("%d\n", stack->size);

	return 0;
}