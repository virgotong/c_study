#ifndef DBLINKLIST_H
#define DBLINKLIST_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct Node 
{
	int element;
	struct Node *pNext;
	struct Node *pPre;	
}NODE, *pNODE;

/* create dblinklist */
pNODE createDblinklist( void );

/* print dblinklist */
void printDblinklist( pNODE pHead );

/* Determine whether the linked list is empty */
int isEmptyDblinklist( pNODE pHead );

/* count length */
int getLengthDblinklist( pNODE pHead );

/* insert */
int insertDblinklist(pNODE pHead, int pos, int data);

/* delete */
int deleteDblinklist(pNODE pHead, int pos);

/* destory */
void destoryDblinklist( pNODE *ppHead );

#endif

