#ifndef SQSTACK_H
#define SQSTACK_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

typedef int INT;
typedef unsigned char BOOL;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// #define MAXSIZE 100
#define EXTEND 10

typedef struct 
{
	INT *bottom;
	INT *top;
	INT stacksize;
	INT realsize;
}SqStack,*pSqStack;

INT initStack(pSqStack s, int size);
INT pushStack(pSqStack s, INT val);
INT popStack(pSqStack s, INT *val);
void getTopElem(pSqStack s, INT *val);
void printStack(pSqStack s);
void destroyStack(pSqStack s);
INT emptyStack(pSqStack s);
INT show_menu(int argc, char *argv[]);

#endif