#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct 
{
	int a[10];
	char *p;	

}test;


int main (int argc, char *argv[])
{
	/*int arr[10] = {1,2,3,4,5,6,6,7,8,9};
	printf("%d %d %d\n", sizeof(arr),sizeof(arr[0]), sizeof(arr[1]));*/
	
	/*char *p = (char *)malloc(128);
	printf("%p\n",p);

	char *ptr = NULL;
	ptr = (char *)realloc(p, 100);
	printf("%p\n", ptr);

	p = ptr;

	if(p) 
	{
		free(p);
	}
	p = NULL;*/

	test a;
	printf("%d\n", sizeof(a));


	return 0;
}





