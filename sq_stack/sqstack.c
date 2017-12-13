#include "sqstack.h"

INT initSatck(pSqStack s, int size)
{
	s->bottom = (int *)malloc(sizeof(int) * size);
	if(s->bottom == NULL)
	{
		printf("Memory allocation failure!\n");
		exit(EXIT_FAILURE);
	}
	s->top = s->bottom;
	s->stacksize = size;
	s->realsize =  0;
	return 0;
}

INT pushStack( pSqStack s, INT val)
{	
	if((s->top - s->bottom) >= s->stacksize)
	{
		printf("栈溢出,已自动扩容！\n");
		s->bottom = (int *)realloc(s->bottom, (s->stacksize + EXTEND) * sizeof(int));
		if(s->bottom == NULL)
		{
			printf("Memory realloc failure!\n");
			exit(EXIT_FAILURE);
		}
		s->top = s->bottom + s->stacksize;
		s->stacksize += EXTEND;
	}
	*s->top++ = val;
	s->realsize++;

	return 0;
}

INT popStack(pSqStack s, int *val)
{
	if(s->top == s->bottom)
	{
		printf("Stack is empty!\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		*val = *--s->top;
		s->realsize--;
	}
	return 0;
}

void getTopElem(pSqStack s, int *val)
{
	if(s->top == s->bottom)
	{
		printf("Stack is empty!\n");
		exit(EXIT_FAILURE);	
	}
	else
	{
		*val = *(s->top -1);
	}	
}

void printStack(pSqStack s)
{		
	while(*(s->top -1) && s->top != s->bottom)
	{		
		printf("%d\n", *(--s->top));		
	}

	printf("\n");
}

INT emptyStack(pSqStack s)
{
	if(s->top == s->bottom)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void destroyStack(pSqStack s)
{
	free(s->bottom);
	s->top = NULL;
	s->bottom = NULL;
	s->stacksize = 0;
	s->realsize = 0;
	printf("Destroy success!\n");
		
}

INT Show_Menu(int argc, char *argv[])        
{  
    int num;  
    printf("********************************************\n");  
    printf("\n  【1】创建            【2】入栈\n");  
    printf("\n  【3】出栈            【4】栈顶\n");  
    printf("\n  【5】打印            【6】销毁\n");  
    printf("\n********************************************\n");  
    printf("请输入你的操作序号：");  
    if(scanf("%d",&num)==0)  
    {  
        printf("请重新输入!\n");  
    }  
    return num;  
}

int main(int argc, char *argv[])
{	
	int flag;
	int n;
	int tmp;
	SqStack a;
	pSqStack stack = &a;

	while(flag != 6)
	{
		flag = Show_Menu(argc, argv);
		switch(flag)
		{
			case 1: 
				printf("Enter the stack size you want to create: ");
				scanf("%d", &n);
				initSatck(stack, n);
				break;

			case 2:
				printf("Enter the size of your data which you want to push into stack: ");
				scanf("%d", &n);				
				while(n--)
				{
					scanf("%d", &tmp);
					pushStack(stack, tmp);
					printf("剩余栈空间为: %d\n", stack->stacksize - stack->realsize);
				}	
								
				break;

			case 3:
				while(!emptyStack(stack))
				{
					popStack(stack, &tmp);
					printf("%d ", tmp);
				}
				printf("Empty Stack!\n");
				break;

			case 4:				
				getTopElem(stack, &tmp);
				printf("栈顶元素: %d\n", tmp);
				break;

			case 5:
				if(!emptyStack(stack))
				{
					printStack(stack);					
				}
				else
				{
					printf("Empty Stack\n");
				}
				break;

			case 6:
				destroyStack(stack);
				break;

			default:
				printf("请输入操作码！\n");
				break;
		}
	}
	return 0;
}
