#include "link_stack.h"

/* 运算符优先级表 */
unsigned char Prior[8][8] =  
{ 
    // '+' '-' '*' '/' '(' ')' '#' '^'   
    /*'+'*/{'>','>','<','<','<','>','>','<'},   
    /*'-'*/{'>','>','<','<','<','>','>','<'},   
    /*'*'*/{'>','>','>','>','<','>','>','<'},   
    /*'/'*/{'>','>','>','>','<','>','>','<'},   
    /*'('*/{'<','<','<','<','<','=',' ','<'},   
    /*')'*/{'>','>','>','>',' ','>','>','>'},   
    /*'#'*/{'<','<','<','<','<',' ','=','<'},   
    /*'^'*/{'>','>','>','>','<','>','>','>'}   
};

char OPSET[8]={'+','-','*','/','(',')','#','^'};

BOOL initStack(pLinkStack S)
{
	S->top = (pStackNode)malloc(sizeof(StackNode));
	if(S->top == NULL)
	{
		printf("Memory of S->top malloc failure!\n");
		return FALSE;
	}
	S->top = NULL;
	S->count = 0;
	return TRUE;
}

BOOL pushStack(pLinkStack S, int val)
{
	pStackNode s = ( pStackNode )malloc(sizeof(StackNode));
	if(s == NULL)
	{
		printf("Memory of s malloc failure!\n");
		return FALSE;
	}
	s->element = val;
	s->next = S->top;
	S->top = s;
	S->count++;
	return TRUE;
}

BOOL popStack(pLinkStack S , int *val)
{
	pStackNode s;
	if(isEmptyStack(S))
	{
		printf("EmptyStack!\n");
		return FALSE;
	}
	else
	{
		*val = S->top->element; //返回出栈元素的值
		s = S->top;
		S->top = S->top->next;
		free(s);
		S->count--;
		//printf("%d\n", *val);
		return TRUE;
	}
}

BOOL isEmptyStack(pLinkStack S)
{
	return S->count == 0 ? TRUE : FALSE;
}

BOOL getTop(pLinkStack S, int *val)
{
	if(S->top == NULL)
	{
		printf("EmptyStack!\n");
		return FALSE;
	}
	else
	{	
		*val = S->top->element;		
		return TRUE;
	}
}

BOOL printStack(pLinkStack S)
{
	pStackNode p;
	p = S->top;
	while(p)
	{
		printf("%d ", p->element);
		p = p->next;
	}
	printf("\n");
	return TRUE;
}

BOOL destroyStack(pLinkStack S)
{
	pStackNode p,q;
	p = S->top;
	while(p)
	{
		q = p;
		free(q);
		p = p->next;
	}
	return TRUE;
}

//SC类型的指针push，返回p
SC *sc_push(SC *s, char c)
{
	SC *p = (SC *)malloc(sizeof(SC));
	p->c = c;
	p->next = s;
	return p;
}

SC *sc_pop(SC *s)
{
	SC *q = s;
	s = s->next;
	free(q);
	return s;	
}

SF *sf_push(SF *s, float f)
{
	SF *p = (SF *)malloc(sizeof(SF));
	p->f = f;
	p->next = s;
	return p;
}

SF *sf_pop(SF *s)
{
	SF *q = s;
	s = s->next;
	free(q);
	return s;
}

//计算函数
float Operate(float a, unsigned char theta, float b)
{
	switch( theta )
	{
		case '+':
			return a + b;
		case '-':
			return a - b;
		case '*':
			return a * b;
		case '/':
			return a / b;
		case '^':
			return pow(a,b);
		default :
			return 0;
	}
}

BOOL findIn( char op1, char *op2)
{
	int i,flag;
	for(i = 0; i < 8; i++)
	{
		if(op1 == op2[i])
		{
			flag = TRUE;
		}		
	}
	return flag;
}

BOOL ReturnOP(char op1, char *op2)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		if( op1 == op2[i] )
		{
			return i;
		}				
	}	
}

char getOP(char op1, char op2)
{
	return Prior[ReturnOP(op1, OPSET)][ReturnOP(op2, OPSET)];
}

float evaluateExpression(char *expression)
{
	SC *optr = NULL;
	SF *optd = NULL;
	
}

int main(int argc, char *argv[])
{
	int i;
	int tmp;
	LinkStack s;
	if(initStack(&s))
	{
		for(i = 0; i < 10; i++)
		{
			pushStack(&s, i+1);
		}
	}

	while(!isEmptyStack(&s))
	{
		popStack(&s, &tmp);
		printf("%d\n", tmp);
	}
	
	//printStack(&s);

	destroyStack(&s);
	printf("Destroy success!\n");

	return 0;
}



