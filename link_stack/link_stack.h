#ifndef LINK_STACK_H
#define LINK_STACK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#ifndef TRUE 
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef unsigned char BOOL;

typedef struct StackNode
{   
    int element;
    struct StackNode *next; 
}StackNode, *pStackNode;

typedef struct 
{
    pStackNode top;
    int count;    
}LinkStack, *pLinkStack;

typedef struct StackChar
{
    char c;
    struct StackChar *next;
}SC;

typedef struct StackFloat
{
    float f;
    struct StackFloat *next;
}SF;

/* initStack */
BOOL initStack(pLinkStack S);

/* pushStack */
BOOL pushStack(pLinkStack S, int val);

/* popStack */
BOOL popStack(pLinkStack S, int *val);

/* isEmptyStack */
BOOL isEmptyStack(pLinkStack S);

/* getTop */
BOOL getTop(pLinkStack S, int *val);

/* printStack */
BOOL printStack(pLinkStack S);

/* destroyStack */
BOOL destroyStack(pLinkStack S);
#endif