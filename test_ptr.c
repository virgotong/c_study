#include <stdio.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char *argv[])
{
	char *p = (char *)malloc(100);
	strcpy(p, "test1");
	printf("one: %s\n", p);
	free(p);
	char *p ="test";	
	printf("%s\n",p);
	return 0;
}

