#include <stdio.h>
#include <stdlib.h>

void print_array(int *p1, int *p2)
{
	printf("%d\n", p2 - p1);
	int i;
	for(i = 0; i < 10; i++)
	{
		printf("%d\n",*(p1 + i));
	}
}

int main(int argc, char *argv[])
{
	int a[10] = {0,1,2,3,4,5,6,7,8,9};
	print_array(&a[0], &a[8]);
	return 0;
}
