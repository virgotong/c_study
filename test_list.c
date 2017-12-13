#include "global.h"

typedef int elemType;

/*structure of single list*/
typedef struct Node {
	elemType element;
	struct Node *next;
}Node;

Node *initList( void )
{
	Node *p;
	p = (Node *)malloc(sizeof(Node));
	if( p == NULL )
	{
		printf("内存分配失败！\n");
		exit(0);
	}
	p->next = NULL;
	return p;
}

void showList(Node *h)
{
	Node *p;
	p = h->next;

	if(p == NULL)
	{
		printf("链表为空\n");
	}
	
	while( p )
	{
		printf("%d", p->element);
		p = p->next;
	}
 	printf("\n");
}

/*头插法*/
void create_list_head( Node *h)
{
	int i, n, num;
	Node *pNew;

	printf("请输入要插入的元素个数: n = ");
	scanf("%d", &n);

	for(i = 0; i < n; i++)
	{
		printf("请输入第%d个数\n", i+1);
		scanf("%d", &num);
		pNew = (Node *)malloc(sizeof(Node));
		if( pNew == NULL )
		{
			printf("内存分配失败(头插法)\n");
			exit(0);
		}
		pNew->element = num;
		pNew->next = h->next;		
		h->next = pNew;
	}
}

/*尾插法*/
void create_list_end(Node *e)
{
	int i, n, num;
	Node *p, *pNew;
	p = e;

	printf("请输入要插入的元素个数: n = ");
	scanf("%d", &n);

	for(i = 0; i < n; i++)
	{
		printf("请输入第%d个数\n", i+1);
		scanf("%d", &num);
		pNew = (Node *)malloc(sizeof(Node));
		if(pNew == NULL)
		{
			printf("内存分配失败(尾插法)\n");
			exit(0);
		}
		pNew->element = num;
		p->next = pNew;
		p = pNew;
	}
	p->next = NULL;
}

void insertList( Node *L)
{
	int i, n, val;
	Node *p, *s;
	p = L;
	i = 1;
	printf("请输入元素插入的位置: n = ");
	scanf("%d", &n);
	printf("请输入你要插入的元素: val = ");
	scanf("%d", &val);

	while( p && i < n)
	{
		p = p->next;
		++i;
	}
	if( !p || i > n )
	{
		printf("无法插入\n");
		exit(0);
	}
	s = (Node *)malloc(sizeof(Node));
	if(s == NULL)
	{
		printf("内存分配失败(insert)\n");
		exit(0);
	}
	s->element = val;
	s->next = p->next;
	p->next = s;
}


void deleteList( Node *L)
{
	int i, n;
	Node *p, *s;
	p = L;
	i = 1;

	printf("请输入你要删除元素的位置: n= ");
	scanf("%d", &n);

	while( p && i < n)
	{
		p = p->next;
		++i;
	}
	if(!(p->next) || i > n)
	{
		printf("删除的位置不合法！\n");
		exit(0);
	}

	s = p->next;
	p->next = s->next;
	free(s);
}

/*查询*/
void queueList( Node *L)
{
	int i, n, val;
	Node *p;
	p = L->next;
	i = 1;

	printf("请输入你要查询的位置: n= ");
	scanf("%d", &n);

	while( p && i < n)
	{
		p = p->next;
		++i;		
	}

	if(!(p->next) || i > n)
	{
		printf("查询位置不合法！\n");
		exit(0);
	}
	val = p->element;
	printf("查询的元素为: %d\n", val);
}

/*销毁*/
void destoryList( Node *L)
{
	Node *p, *s;
	p = L->next;
	while( p )
	{
		s = p->next;
		free(p);
		p = s;
	}
	L->next = NULL;
}

/*导航菜单*/
int Show_Menu()        
{  
    int num;  
    printf("********************************************\n");  
    printf("\n  【1】创建            【2】插入\n");  
    printf("\n  【3】删除            【4】查询\n");  
    printf("\n  【5】打印            【6】退出\n");  
    printf("\n********************************************\n");  
    printf("请输入你的操作序号：");  
    if(scanf("%d",&num)==0)  
    {  
        printf("请重新输入!\n");  
    }  
    return num;  
}  


int test_list( int argc, char *argv[])
{
	int flag = 0;
	Node *pList ;
	pList = initList();

	if( pList == NULL )
	{
		printf("初始化失败！\n");
	}

	while(flag != 6)
	{
		flag = Show_Menu();
		switch( flag )
		{
			case 1: 
				create_list_head( pList );
				break;

			case 2:
				insertList( pList );
				break;

			case 3:
				deleteList( pList );
				break;

			case 4: 
				queueList( pList );
				break;

			case 5:
				showList( pList );
				break;

			case 6:
				destoryList( pList );
				exit(0);
				break;

			default:
				exit(0);
				break;
		}
	}
}