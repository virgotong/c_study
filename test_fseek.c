#include <stdio.h>
#include <malloc.h>
#include <string.h>

int main()
{
	FILE *fp = fopen("/home/librdkafka-master/examples/rate_buf.db", "r") ;

	fseek (fp, 0L, SEEK_END) ;
	long size = ftell(fp) ;
	printf ("%ld\n", size) ;

	const int step=10000 ;
	int start = 0, end = size-1 ;
	char *buf = (char *)malloc(step) ;
	fseek (fp, 0, SEEK_SET) ;
	while (start <= end)
	{
		memset (buf, 0, step) ;
		fread (buf, step, 1, fp) ;
		buf[step] = '\0' ;
		printf ("%ld+%d: %s\n", start, step, buf) ;
		start += step ;
	}
	/*if (start != end)
	{
		memset (buf, 0, step) ;
		int step_end = end - start ;
		//fseek (fp, start, SEEK_SET) ;
		fread (buf, step_end, 1, fp) ;
		buf[step_end] = '\0' ;
		printf ("%ld+%d: %s\n", start, step_end, buf) ;
	
	}*/

	fclose (fp) ;
	if( buf )
	{
		free( buf );
	}

	return 0 ;
}
