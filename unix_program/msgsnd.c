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

int main( int argc, char *argv[] )
{
	int msg_id;
	Msgs msg_data;
	char buffer[ MAX_LENGTH ];

	if( ( msg_id = msgget( ( key_t )MSG_KEY, 0666 | IPC_CREAT ) ) < 0 )
	{
		printf("Failed creating message queue: %s\n", strerror( errno ));
		exit( 0 );
	}

	while( 1 )
	{
		printf("Enter some string: \n");
		fgets( buffer, MAX_LENGTH, stdin );

		msg_data.msg_type = 1;
		strcpy( msg_data.buf, buffer );

		msgsnd( msg_id, &msg_data, sizeof( msg_data ) - sizeof( msg_data.msg_type ), IPC_NOWAIT );

		sleep( 2 );
	}

	return 0;

}