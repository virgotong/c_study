#include <stdio.h>
#include <string.h>

int a_to_i(char *s)
{
	int sign = 1;
	int m = 0;
	if(*s == '+' || *s == '-')
	{
		sign = (*s++ == '+') ? 1 : -1;		
	}
	while((*s)!='\0')
	{
		m = m*10 + (*s - '0');		
		s++;
	}
	printf("m: %d sign: %d\n",m,sign);
	return m*sign;
}

int main()
{
	char *p = "-111";
	//int i =  atoi(p);
	int i = a_to_i(p);
	printf("%d\n",i);

}
