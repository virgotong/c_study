#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int cmp(const void *param1, const void *param2)
{
	return *(int*)param1 - *(int*)param2;
}


int main(int argc, char *argv[])
{
	int i;
	int a[10] = {1,2,3,12,215,56456,47,5,4,5};
	int *p = a;

	qsort(p,10,sizeof(p),cmp);
	for(i = 0; i < 10; i++)
	{
		printf("%d\n", a[i]);
	}

	return 0;
}
