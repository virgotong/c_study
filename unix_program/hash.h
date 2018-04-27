#ifndef __HASH_H__
#define __HASH_H__

#define HASH_TABLE_SIZE		256
#define HASH_TABLE_INDEX( index )	( ( ( index ) ^ ( ( index ) >> 12 ) ^ ( ( index ) >> 24 ) ) & ( HASH_TABLE_SIZE - 1 ) )

typedef unsigned long ( *HASH_FUNC )( const void *key, int key_size );
typedef int ( *HASH_COMPARE )( const void *key1, int key1_size, const void *key2, int key2_size );

typedef struct hash_object
{
	struct hash_object	*next;
	int key_size;
	int data_size;

	unsigned char 		object_buffer[ 0 ];
}lz_hash_object;

typedef struct
{
	HASH_FUNC 		hash_func;
	HASH_COMPARE	hash_compare;

	lz_hash_object *	hash_table[ HASH_TABLE_SIZE ];
} hash_map;

unsigned long hmap_hash_string( const void *key, int key_size );
int hmap_compare_string( const void *key1, int key1_size, const void *key2, int key2_size );

void init_hash_map( hash_map *map, HASH_FUNC hash_func, HASH_COMPARE hash_compare );
void hash_map_free( hash_map *map );

int hash_map_find( hash_map *map, const void *key, int key_size, void **data_buffer, int *data_size );
int hash_map_insert( hash_map *map, const void *key, int key_size, void **data_buffer, int *data_size );
int hash_map_remove( hash_map *map, const void *key, int key_size );

#endif