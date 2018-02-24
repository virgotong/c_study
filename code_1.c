#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

void init_str( char **sbuf )
{
	char *s = "asncauicn";
	char *str = (char *)malloc( 100 );
	memcpy(str, s, strlen(s));

	*sbuf = str;

	if( str )
	{
		free(str);
	}

	printf("sbuf: %s\n", *sbuf);
}

int main( int argc, char *argv[] )
{
	char *buf = NULL;
	init_str( &buf );

	printf("buf: %s\n", buf);

	return 0;
}
