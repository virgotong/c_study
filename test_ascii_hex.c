#include <stdio.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char *argv[])
{
	char p[10] = {1,2,3,4};
	printf("%02x\n", p[0]);
	return 0;
}
