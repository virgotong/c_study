#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char *get_key( char *str )
{
	snprintf( str, 32, "%s", "ajbcahcasxa" );
	return str;
}

int main()
{
	char key[32];
	get_key( key );
	printf( "key: %s\n", key );
	return 0;
}
