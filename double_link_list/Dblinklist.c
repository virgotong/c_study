#include "Dblinklist.h"

pNODE createDblinklist( void )
{
	int i;
	int length = 0;
	int element = 0;

	pNODE pTail = NULL;
	pNODE pNew = NULL;

	pNODE pHead = (pNODE)malloc(sizeof(NODE));

	if( pHead == NULL )
	{
		printf("内存分配失败！\n");
		exit(0);
	}

	pHead->element = 0;
	pHead->pNext = NULL;
	pHead->pPre = NULL;
	pTail = pHead;

	printf("请输入链表的长度: ");
	scanf("%d", &length);

	for( i = 0; i < length; i++)
	{
		pNew = (pNODE)malloc(sizeof(NODE));
		if( pNew == NULL )
		{
			printf("pNew内存分配失败\n");
			exit(0);
		}

		printf("请输入第%d个元素: ", i+1);
		scanf("%d", &element);

		pNew->element = element;
		pNew->pNext = NULL;
		pNew->pPre = pTail;
		pTail->pNext = pNew;
		pTail = pNew;
	}

	return pHead;
}

void printDblinklist( pNODE pHead )
{
	pNODE ptr = pHead->pNext;
	printf("链表信息如下：\n");
	while( ptr != NULL )
	{
		printf("%d ", ptr->element);
		ptr = ptr->pNext;
	}

	printf("\n");
}

int emptyDblinklist( pNODE pHead )
{
	pNODE ptr = pHead->pNext;

	if(ptr = NULL)
	{	
		return 1;
	}
	else
	{
		return 0;
	}
}

int getLengthDblinklist( pNODE pHead )
{
	int length = 0;
	pNODE ptr = pHead->pNext;

	while( ptr != NULL )
	{
		length++;
		ptr = ptr->pNext;
	}
	return length;
}

int insertDblonklist(pNODE pHead, int pos, int data)
{
	pNODE pt  = NULL;
	pNODE p_new = NULL;

	if(pos > 0 && pos < getLengthDblinklist(pHead)+2)
	{
		p_new = (pNODE)malloc(sizeof(NODE));
		if( p_new == NULL )
		{
			printf("p_new内存分配失败！\n");
			exit(0);
		}

		while( 1 )
		{
			pos--;
			if(pos == 0)
				break;
			pHead = pHead->pNext;
		}

		pt = pHead->pNext;
		p_new->element = data;
		p_new->pNext = pt;

		if(pt != NULL) // 判断最后的节点是否为空
			pt->pPre = p_new;
		p_new->pPre = pHead;
		pHead->pNext = p_new;

		return 1;
	}
	else
	{
		return 0;
	}
}

int deleteDblinklist(pNODE pHead, int pos)
{
	pNODE pt = NULL;

	if(pos > 0 && pos < getLengthDblinklist(pHead) + 1)
	{
		while( 1 )
		{
			pos--;
			if(pos == 0)
				break;
			pHead = pHead->pNext;
		}

		pt = pHead->pNext->pNext;
		free(pHead->pNext);
		pHead->pNext = pt;
		if(pt != NULL)
		{
			pt->pPre = pHead;
		}

		return 1;
	}
	else
	{
		return 0;
	}
}

void destoryDblinklist( pNODE *ppHead )
{
	pNODE pt = NULL;

	while( *ppHead != NULL )
	{
		pt = (*ppHead)->pNext;
		free(*ppHead);
		if(pt != NULL )
		{
			pt->pPre = NULL;
		}
		*ppHead = pt;
	}
}

/*导航菜单*/
int Show_Menu()        
{  
    int num;  
    printf("********************************************\n");  
    printf("\n  【1】创建            【2】打印\n");  
    printf("\n  【3】长度            【4】插入\n");  
    printf("\n  【5】删除            【6】销毁\n");  
    printf("\n********************************************\n");  
    printf("请输入你的操作序号：");  
    if(scanf("%d",&num)==0)  
    {  
        printf("请重新输入!\n");  
    }  
    return num;  
}

int main( int argc, char *argv[])
{
	int flag;
	int length = 0;
	int pos = 0;
	int value = 0;
	int val = 0;

	pNODE head = NULL;

	while( flag != 6)
	{
		flag = Show_Menu();
		switch( flag )
		{
			case 1: 
				head = createDblinklist();				
				break;
			case 2:
				if( head == NULL )
				{
					printf("请先创建链表！\n");
				}
				else
				{
					printDblinklist(head);
				}
				break;
			case 3:
				val = emptyDblinklist(head);
				if(val)
				{
					printf("链表为空！\n");
				}
				else
				{
					length = getLengthDblinklist(head);
					printf("链表长度为: %d\n", length);
				}
				break;
			case 4:
				printf("请输入要插入节点的位置和元素的值，并用空格隔开\n");
				scanf("%d %d", &pos, &value);
				val = insertDblonklist(head, pos, value);
				if(val)
				{
					printf("插入元素成功！\n");
					printDblinklist(head);
				}
				else
				{
					printf("插入元素失败！\n");
				}
				break;
			case 5:
				val = emptyDblinklist(head);
				if(val)
				{
					printf("链表为空，不能进行删除操作！\n");					
				}
				else
				{
					printf("请输入要删除节点的位置！\n");
					scanf("%d", &pos);
					val = deleteDblinklist(head, pos);
					if(val)
					{
						printf("删除节点成功！\n");
						printDblinklist(head);
					}
					else{
						printf("删除节点失败！\n");
					}
				}
				break;
			case 6:
				destoryDblinklist(&head);
				if(head == NULL)
				{
					printf("链表销毁成功！\n");
				}
				else
				{
					printf("链表销毁失败！\n");
				}
				break;
			default:
				printf("请输入操作码！\n");
				break;
		}
	}
	return 0;
}
