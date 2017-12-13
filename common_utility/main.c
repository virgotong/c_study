#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commonUtility.h"

int main(int argc, char *argv[])
{	
	char c = 'F';
	int rval = my_hex_value(c);
	printf("%d\n", rval);
	return 0;
}