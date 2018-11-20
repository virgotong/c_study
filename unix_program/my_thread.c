/*************************************************************************
	> File Name: pthread.c
	> Author: sixingdong
	> Created Time: Wed 29 Aug 2018 03:47:18 PM CST
 ************************************************************************/
#include "common.h"

#define MAX_THREAD_NUM	3
#define MAX_LEN			20
pthread_mutex_t mutex;
#define ZERO_DATA( x )	memset( &( x ), 0, sizeof( x ) )
static int s_index = 0;
static int count = 0;

typedef struct _LIST_ENTRY
{
	struct _LIST_ENTRY *Flink;
	struct _LIST_ENTRY *Blink;

} LIST_ENTRY, *PLIST_ENTRY;

#define LIST_ENTRY_INIT( h )						{ &( h ), &( h ) }


static inline void RemoveEntryList( PLIST_ENTRY Entry )
{
	PLIST_ENTRY Blink;
	PLIST_ENTRY Flink;

	Flink = Entry->Flink;
	Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;
}

static inline void InsertTailList( PLIST_ENTRY ListHead, PLIST_ENTRY Entry )
{
	PLIST_ENTRY Blink;

	Blink = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Blink;
	Blink->Flink = Entry;
	ListHead->Blink = Entry;
}

typedef struct
{
	char *name;
	int age;
}Students, *PStudents;

void print_stu( PStudents data, int index )
{
	int i;
	for( i = 0; i < 90000; i++ )
	{
		int count;
		count = i + i * 173 / 24;
	}

	printf( "print students data[%d]: name->%s age->%d\n", index, data->name, data->age );
	return;
}

void *print_data( void *data )
{
	int itr;	
	PStudents params = ( PStudents )( data );

	#if 1
	while( s_index < MAX_LEN )
	{
		int current_index;
		pthread_mutex_lock( &mutex );
		current_index = s_index++;		
		pthread_mutex_unlock( &mutex );

		if( current_index < MAX_LEN )
		{
			print_stu( &params[ current_index ], current_index );
		}		
	}
	#endif

	return NULL;
}

void print_detail( PStudents params )
{
	int itr;	
	pthread_t thread[ MAX_THREAD_NUM ];

	pthread_mutex_init (&mutex, NULL);

	for( itr = 0; itr < MAX_THREAD_NUM; itr++ )
	{
		if( pthread_create( &thread[ itr ], NULL, print_data, (void *)params ) !=0 )
		{
			printf( "Failed to create thread[ %d ]\n", itr );
		}
	}

	for( itr = 0; itr < MAX_THREAD_NUM; itr++ )
	{
		if( pthread_join( thread[ itr ], NULL ) != 0 )
		{
			printf("Failed to join thread[ %d ]\n", itr);
		}

		//printf( "res: %d\n", ( int )res );
	}

	pthread_mutex_destroy(&mutex);

	return;
}

void init_students( void )
{
	int i;
	Students *s_data = NULL;
	if( !( s_data = malloc( ( MAX_LEN ) * sizeof( Students ) ) ) )
	{
		printf("Malloc failed!\n");
	}

	char *buf[ MAX_LEN ] = {"Stu1", "Stu2", "Stu3", "Stu4", "Stu5", "Stu6", "Stu7", "Stu8", "Stu9", "Stu10", "Stu11", "Stu12", "Stu13", "Stu14", "Stu15", "Stu16", "Stu17", "Stu18", "Stu19", "Stu20"};

	for( i = 0; i < MAX_LEN; i++ )
	{
		ZERO_DATA( s_data[ i ] );
		s_data[i].name = buf[ i ];
		s_data[i].age = i + 10;
	}

	print_detail( s_data );

	if( s_data )
	{
		free( s_data );
	}
}

void add()
{
	count += 10;

	return;
}

int main( int argc, char *argv[] )
{
	#if 1
	init_students( );
	#endif

	#if 0
	int cid;
	cid = fork();

	if( cid == 0 )
	{
		add( );		
		sleep(1);
		printf("count: %d pid: %d ppid: %d\n", count, getpid(), getppid());
		exit(0);
	}

	printf("count: %d\n", count);
	#endif

	return 0;
}
