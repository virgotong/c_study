#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int daemonize( const char *lock_file )
{
	int pid, fd;

	if( ( pid = fork( ) ) == 0 )
	{
		// start a new process group
		setsid( );

		// close stdin, stdout and stderr
		// close( 0 );
		// close( 1 );
		// close( 2 );

		// reopen stdin on /dev/null. and 'dup' to stdout and stderr
		fd = open( "/dev/null", O_RDWR );
		dup( fd ); dup( fd );
		chdir( "/" );

		if( lock_file )
		{
			char temp[256];

			// make sure only single daemon running
			fd = open( lock_file, O_RDWR | O_CREAT, 0640 );
			if( fd < 0 )exit( 1 );
			if( lockf( fd, F_TLOCK, 0 ) < 0 )exit( 0 );
			sprintf( temp, "%d\n", getpid( ) );
			write( fd, temp, strlen( temp ) );
		}		
	}	
	return pid;
}

/*
 *note: fork函数调用之后会返回两次，有三种不同的值：
 *<1>在父进程中，fork返回新创建子进程的进程ID；
 *<2>在子进程中，fork返回0;
 *<3>如果出现错误，fork返回一个负值；
 */
void init_daemonize( void )
{
	int flags = daemonize( "/tmp/pid_1.lock "); // 会返回两个值，一个是0，一个是子进程的ID

	/* 此处进入到子进程当中 */
	if( flags == 0 )
	{
		printf("*****Start Child Process*****\n");
		printf( "pid_1: %d\n", getpid( ) );
		printf("*****End Child Process*****\n");	
		printf("\n");

		return; //子进程调用结束，函数返回
	}

	printf("flags: %d\n", flags); //fork之后，父进程返回新建子进程的进程ID，所以此处打印的是子进程的ID
	printf( "main_pid: %d\n", getpid( ) );
}

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
	// char *buf = NULL;
	// init_str( &buf );

	// printf("buf: %s\n", buf);
	// printf("*****Start Main Process*****\n");
	// printf( "main_pid: %d\n", getpid( ) );
	// init_daemonize( );
	// printf("*****End Main Process*****\n");
	// printf("\n");

	/* test setsid() */
	if( fork( ) > 0 )
	{
		printf("Parent process is start!\n");

		sleep( 10 );

		printf("Parent process is exit\n");

		exit( 0 );
	}

	setsid( );

	printf("Child process is start!\n");

	sleep( 100 );

	printf("Child process is exit!\n");

	return 0;
}
