#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
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

void init_hash_map( hash_map *map, HASH_FUNC hash_func, HASH_COMPARE hash_compare )
{
	int itr;
	map->hash_func = hash_func;
	map->hash_compare = hash_compare;

	for( itr = 0; itr < HASH_TABLE_SIZE; itr++ )
	{
		map->hash_table[ itr ] = NULL;
	}
}

void hash_map_free( hash_map *map )
{
	int itr;
	lz_hash_object	*obj, *next;

	for( itr = 0; itr < HASH_TABLE_SIZE; itr++ )
	{
		for( obj = map->hash_table[ itr ]; obj; obj = next )
		{
			next = obj->next;
			free( obj );
		}
		map->hash_table[ itr ] = NULL;
	}
}

int hash_map_find( hash_map *map, const void *key, int key_size, void **data_buffer, int *data_size )
{
	unsigned long hval;
	lz_hash_object *obj;

	hval = map->hash_func( key, key_size );
	hval = HASH_TABLE_INDEX( hval );

	for( obj = map->hash_table[ hval ]; obj; obj = obj->next )
	{
		if( map->hash_compare( obj->object_buffer, obj->key_size, key, key_size ) == 0 )
		{
			*data_buffer = obj->object_buffer + obj->key_size;
			*data_size = obj->data_size;

			return 0;
		}
	}

	return -ENOENT;
}

int hash_map_insert( hash_map *map, const void *key, int key_size, const void *data_buffer, int data_size)
{
	unsigned long hval;
	lz_hash_object	**ptr, *obj, *next = NULL;

	hval = map->hash_func( key, key_size );
	hval = HASH_TABLE_INDEX( hval );

	for( ptr = &map->hash_table[ hval ]; *ptr; ptr = &( *ptr )->next )
	{
		obj = *ptr;
		if( map->hash_compare( obj->object_buffer, obj->key_size, key, key_size ) == 0 )
		{
			if( obj->key_size == key_size && obj->data_size == data_size )
			{
				memcpy( obj->object_buffer + key_size, data_buffer, data_size );
				return 0;
			}

			next = obj->next;
			free( obj );
			break;
		}
	}

	obj = ( lz_hash_object *)malloc( sizeof( *obj ) + key_size + data_size );
	if( !obj ) return -ENOENT;

	obj->next = next;
	obj->key_size = key_size;
	obj->data_size = data_size;
	memcpy( obj->object_buffer, key, key_size );
	memcpy( obj->object_buffer + key_size, data_buffer, data_size );

	*ptr = obj;
	return 0;
}

int hash_map_remove( hash_map *map, const void *key, int key_size )
{
	unsigned long hval;
	lz_hash_object **ptr, *obj;

	hval = map->hash_func( key, key_size );
	hval = HASH_TABLE_INDEX( hval );

	for( ptr = &map->hash_table[ hval ]; *ptr; ptr = &( *ptr )->next )
	{
		obj = *ptr;
		if( map->hash_compare( obj->object_buffer, obj->key_size, key, key_size ) == 0 )
		{
			*ptr = obj->next;
			free( obj );
			return 0;
		}
	}

	return -ENOENT;
}

int main( int argc, char *argv[] )
{
	unsigned char buff[0];
	memcpy( buff, "a", 1);
	printf("%s\n", buff);

	return 0;
}