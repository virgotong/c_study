#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

static inline void my_strlcpy( char *dst, const char *src, int size )
{
		int	n = size;

			while( -- n > 0 )
						if( ! ( *dst ++ = *src ++ ) )break;

				if( n <= 0 && size > 0 )*dst = 0;
}

/* my_strlcat -- safe concat string */
static inline void my_strlcat( char *dst, const char *src, int size )
{
		while( size > 1 && *dst )dst ++, size --;
			my_strlcpy( dst, src, size );
}


int main(int argc, char*argv[])
{
	char *key_name = "IpLinkFlow@";
	char *srand_num = "1467815";
	char *key = (char *)malloc(40);
	my_strlcat(key, key_name, 40);
	my_strlcat(key, srand_num, 40);
	printf("key: %s\n", key);
	return 0;
}




