#include <stdio.h>
#include <time.h>

#define GET_TIMESTAMP()  time( NULL )

int main( void )
{
	time_t ts = GET_TIMESTAMP( );
	struct tm tblock = *localtime(&ts);
	printf("ts: %d\n", ts);
	printf("tblock: %s\n", asctime(&tblock));
}
