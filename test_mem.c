#include <stdio.h>
#include <malloc.h>
#include <string.h>

int main()
{
/*	int i;
	char a[] = {'0','1','2','3','4','5','6','7','8','9'};
	
	memcpy(a+4, a, sizeof(int)*6);
	for(i = 0; i < 10; i++)
	{
		printf("%c", a[i]);
	}
	printf("%d\n",sizeof(int)*6);*/
	
	char buf[20] = "hello world\n";
	memcpy(buf + 1, buf, 13);
	printf(buf);
	return 0;
}
