#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <errno.h>

#define MAX_LENGTH	512
#define MSG_KEY		1234

typedef struct
{
	int msg_type;
	char buf[ MAX_LENGTH ];
} Msgs;

int main( int aggc, char *argv[] )
{
	int msg_id;
	Msgs msg_data;

	if( ( msg_id = msgget( ( key_t )MSG_KEY, 0666 | IPC_CREAT ) ) < 0 )
	{
		printf("Failed creating message queue: %s\n", strerror( errno ));
		exit( 0 );
	}

	while( 1 )
	{
		if( msgrcv( msg_id, &msg_data, sizeof( msg_data ) - sizeof( msg_data.msg_type ), 1, IPC_NOWAIT ) < 0 )
		{
			printf("msgrcv failed: %s\n", strerror( errno ) );			
		}

		printf("Recieve the string: %s\n", msg_data.buf);

		sleep( 2 );
	}

	return 0;
}