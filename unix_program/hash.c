#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "hash.h"

static inline int my_locase( int c )
{
	return c >='A' && c <='Z' ? c - 'A' + 'a' : c;
}

static inline int my_cmp( const char *a, const char *b, int len )
{
	register char la, lb;

	while( len > 0 && ( ( la = ( char )my_locase( *a ) ) == ( lb = ( char )my_locase( *b ) ) ) )
	{
		a++;
		b++;
		len--;
	}
	return len < 0 ? 0 : ( int )( la -lb );
}

unsigned long hmap_hash_string( const void *key, int key_size )
{
	unsigned long h = 0, g;
	unsigned char *key_str = ( unsigned char *)key;

	while( key_size-- > 0 )
	{
		h = ( h << 4 ) + *key_str ++;
		if( ( g = ( h & 0xF0000000 ) ) )
		{
			h = h ^ ( g >> 24 );
			h = h ^ g;
		}
	}

	return h;
}

int hmap_compare_string( const void *key1, int key1_size, const void *key2, int key2_size )
{
	if( key1_size != key2_size ) return 1;
	return my_cmp( key1, key2, key1_size );
}

int main( int argc, char *argv[] )
{
	const char *key1 = "abcasc";
	const char *key2 = "akmcaiscnas";

	int result = hmap_compare_string( key1, strlen( key1), key2, strlen( key2 ) );
	printf("result: %d\n", result);

	return 0;
}

