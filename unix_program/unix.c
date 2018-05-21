// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <signal.h>
// #include <execinfo.h>
// #include <pwd.h>
// #include <shadow.h>
// #include <sys/time.h>

#include "common.h"

#define ZERO_DATA( x )	memset( &( x ), 0, sizeof( x ) )

typedef struct
{
	//char name[256];
	const char *name;
	int age;
} Students;

static inline int my_locase( char c )
{
	return c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
}

static inline int my_stricmp( const char *a, const char *b )
{
	register char la, lb;

	while( ( la = ( char )my_locase( *a ) ) == ( lb = ( char )my_locase( *b ) ) && *a && *b )
	{
		a ++;
		b ++;
	}

	return ( int )( la - lb );
}

void display_trace( void )
{
	int itr, trace_size;
	void *trace[ MAX_TRACE_SIZE ];
	char **messages = NULL;

	trace_size = backtrace( trace, MAX_TRACE_SIZE );
	messages = backtrace_symbols( trace, trace_size );

	printf("The stack tarce is ....\n");
	for( itr = 0; itr < trace_size; itr ++ )
	{
		printf("[%02d] %s\n", itr, messages[itr]);
	}

	free( messages );
}

void fault_handler( int sig, siginfo_t *info, void *ucontext )
{
	/* this structure mirrors the one found in /usr/include/asm/ucontext.h */
	typedef struct _sig_ucontext {
		unsigned long     uc_flags;
		struct ucontext   *uc_link;
		stack_t           uc_stack;
		struct sigcontext uc_mcontext;
		sigset_t          uc_sigmask;
	} sig_ucontext_t;

	sig_ucontext_t *uc = ( sig_ucontext_t * )ucontext;
	printf( "SIGNAL %d ( %s ) !!! address %p from %p",
		sig, strsignal( sig ), info->si_addr,
		#if defined( __tilera__ )
		( void * )uc->uc_mcontext.pc
		#elif defined( __LP64__ ) || defined( _WIN64 )
		( void * )uc->uc_mcontext.rip
		#else
		( void * )uc->uc_mcontext.eip
		#endif
	);
	display_trace( );
	_exit( sig );
}

void install_fault_handler( void )
{
	struct sigaction	sa;
	
	// register the signal handlers, to show the trace at fault
	sa.sa_sigaction = ( void *)fault_handler;
	sigemptyset( &sa.sa_mask );
	sa.sa_flags = SA_RESTART | SA_SIGINFO;

	sigaction( SIGSEGV	, &sa, NULL );
	sigaction( SIGABRT	, &sa, NULL );
	sigaction( SIGBUS	, &sa, NULL );
	sigaction( SIGILL	, &sa, NULL );
	sigaction( SIGFPE	, &sa, NULL );
}

/*
 *测试passwd结构体
 *struct passwd
  {
    char * pw_name; //用户账号
    char * pw_passwd; //用户密码
    uid_t pw_uid; //用户识别码
    gid_t pw_gid; //组识别码
    char * pw_gecos; //用户全名
    char * pw_dir; //家目录
    char * pw_shell; //所使用的shell 路径
  };
 *
 */
void get_passwd( void )
{
	struct passwd 	*user;
	struct spwd		*sp;
	setpwent( );

	while( ( user = getpwent( ) ) )
	{		
		if( my_stricmp( user->pw_passwd, "x") == 0 || my_stricmp( user->pw_passwd, "*") == 0 )
		{			
			sp = getspnam( user->pw_name );
			printf("sp_name: %s sp_pass: %s\n", sp->sp_namp, sp->sp_pwdp);
		}
		else
		{
			printf("name: %s pass: %s dir: %s shell: %s\n", user->pw_name, user->pw_passwd, user->pw_dir, user->pw_shell );	
		}
		
	}

	return;
}

int get_param( int argc, char *argv[] )
{
	int index, nextopt;

	for( index = 1; index < argc; )
	{
		if( argv[ index ][ 0 ] != '-' )
		{
			index++;			
			continue;
		}

		for( nextopt = index + 1; nextopt < argc && argv[ nextopt ][ 0 ] != '-'; nextopt ++ );			

		if( strcmp( argv[ index ], "-i" ) == 0 )
		{
			printf("argv: %s\n", argv[ index ]);
			if( nextopt - index > 3 )
			{				
				printf("%s-%s-%s\n", argv[ index +1 ], argv[ index + 2 ], argv[ index + 3 ] );
			}
		}

		index = nextopt;
	}

	return 0;
}

static inline int get_socket_option( int sk, int level, int name )
{
	int			val = -1;
	socklen_t	val_len = sizeof( val );

	// if the 'sk' does not support getting the option, treat as 'succeed'
	return getsockopt( sk, level, name, &val, &val_len ) >= 0 ? val : 0;
}

int my_wait_write( int fd, int timeout )
{
	int				result;
	fd_set			fds;
	struct timeval	start_tv, tv;

	FD_ZERO( &fds );
	FD_SET( fd, &fds );

	tv.tv_sec	= timeout / 1000;
	tv.tv_usec	= ( timeout % 1000 ) * 1000;

	gettimeofday( &start_tv, NULL );
	result = select( fd + 1, NULL, &fds, NULL, &tv );

	// result == 0, select timed out; result < 0, error in select
	if( result <= 0 )return result;

	// the 'select' may result positive if the connection gets some error, and the 'getsockopt'
	// with 'SO_ERROR' option name would tell the actual error code. so verify it after select
	result = get_socket_option( fd, SOL_SOCKET, SO_ERROR );
	if( result )return -result;

	// result > 0, something can be write, return the time left
	gettimeofday( &tv, NULL );
	tv.tv_sec -= start_tv.tv_sec;
	if( tv.tv_usec < start_tv.tv_usec )
		tv.tv_sec --, tv.tv_usec += USECS_PER_SECOND;
	tv.tv_usec -= start_tv.tv_usec;

	timeout -= tv.tv_sec * 1000 + tv.tv_usec / 1000;
	if( timeout <= 0 )timeout = 1;	// keep the result positive

	return timeout;
}

int my_wait_read( int fd, int timeout )
{
	int				result;
	fd_set			fds;
	struct timeval	start_tv, tv;

	// from linux.die.net : On Linux, select() modifies timeout to reflect the amount
	// of time not slept; most other implementations do not do this. (POSIX.1-2001 permits
	// either behavior.) This causes problems both when Linux code which reads timeout is
	// ported to other operating systems, and when code is ported to Linux that reuses a
	// struct timeval for multiple select()s in a loop without reinitializing it. Consider
	// timeout to be undefined after select() returns.

	// the tilera system ports the glibc and it uses 'pselect' to simulate 'select', it
	// does not update the timeout value. so use the following implementation to work around

	FD_ZERO( &fds );
	FD_SET( fd, &fds );

	tv.tv_sec	= timeout / 1000;
	tv.tv_usec	= ( timeout % 1000 ) * 1000;

	gettimeofday( &start_tv, NULL );
	result = select( fd + 1, &fds, NULL, NULL, &tv );

	// result == 0, select timed out; result < 0, error in select
	if( result <= 0 )return result;

	// the 'select' may result positive if the connection gets some error, and the 'getsockopt'
	// with 'SO_ERROR' option name would tell the actual error code. so verify it after select
//	result = get_socket_option( fd, SOL_SOCKET, SO_ERROR );
//	if( result )return -result;

	// result > 0, something can be read, return the time left
	gettimeofday( &tv, NULL );
	tv.tv_sec -= start_tv.tv_sec;
	if( tv.tv_usec < start_tv.tv_usec )
		tv.tv_sec --, tv.tv_usec += USECS_PER_SECOND;
	tv.tv_usec -= start_tv.tv_usec;

	timeout -= tv.tv_sec * 1000 + tv.tv_usec / 1000;
	if( timeout <= 0 )timeout = 1;	// keep the result positive

	return timeout;
}

int shell_output( PBYTE *outbuf, int *outlen, PBYTE inbuf, int inlen, const char *path, ... )
{
	#define REALLOC_STEP	0x1000
	#define WRITE_STEP		0x1000
	#define READ_STEP		0x100

	int	result = -1, cid;
	int	ifds[ 2 ] = { -1, -1 }, ofds[ 2 ] = { -1, -1 };

	// some cgi server do not set this, and we need it to
	// execute the tools. note the 'overwrite' is '0'
	setenv( "PATH", "/bin:/sbin:/usr/bin:/usr/sbin", 0 );
	// must flush the cached data. otherwise the output before
	// the fork would be flushed by both parent and the child
	fflush( NULL );

	if( outbuf )*outbuf = NULL;
	if( outlen )*outlen = 0;

	if( pipe( ifds ) >= 0 &&
		pipe( ofds ) >= 0 )
	{
		cid = fork( );
		if( cid == 0 )
		{
			// close the useless end of the pipe
			close( ifds[ 1 ] );
			close( ofds[ 0 ] );

			if( dup2( ifds[ 0 ], STDIN_FILENO ) >= 0 &&
				dup2( ofds[ 1 ], STDOUT_FILENO ) >= 0 &&
				dup2( ofds[ 1 ], STDERR_FILENO ) >= 0 )
			{
				va_list	ap;
				int		argc;
				PCSTR	argv[ MAX_EXECUTE_ARGS_SIZE ];

				// duplicated done, decrease the reference of the pipe descriptors
				close( ifds[ 0 ] );
				close( ofds[ 1 ] );

				// parse and generate the argument list
				va_start( ap, path );

				argv[ 0 ] = path;
				for( argc = 1; argc < MAX_EXECUTE_ARGS_SIZE - 1; argc ++ )
				{
					argv[ argc ] = va_arg( ap, PCSTR );
					if( ! argv[ argc ] )break;
				}

				argv[ argc ] = NULL;
				va_end( ap );

				// execute the command
				result = execvp( path, ( char * const * )argv );
			}

			// if failed
			close( ifds[ 0 ] );
			close( ofds[ 1 ] );
			exit( result );
		}
		else if( cid > 0 )
		{
			int		res, tsize, buffer_size = 0, buffer_pos = 0;
			PBYTE	buffer = NULL;

			// close the useless end of the pipe
			close( ifds[ 0 ] );
			close( ofds[ 1 ] );

			while( TRUE )
			{
				// write data to the child
				if( inbuf && inlen > 0 )
				{
					res = my_wait_write( ifds[ 1 ], 0 );
					if( res < 0 )break;
					if( res > 0 )
					{
						tsize = inlen;
						if( tsize > WRITE_STEP )tsize = WRITE_STEP;
						if( write( ifds[ 1 ], inbuf, tsize ) != tsize )break;
						inlen -= tsize;
						inbuf += tsize;
					}
				}
				else
				{
					// tell the child that no more data
					if( ifds[ 1 ] > 0 )
					{
						close( ifds[ 1 ] );
						ifds[ 1 ] = -1;
					}
				}

				res = my_wait_read( ofds[ 0 ], 0 );
				if( res < 0 )break;
				if( res > 0 )
				{
					// try to read data from the child
					if( buffer_pos + READ_STEP > buffer_size )
					{
						PBYTE buffer2 = realloc( buffer, buffer_size + REALLOC_STEP );
						if( ! buffer2 )break;	// no more memory available

						buffer = buffer2;
						buffer_size += REALLOC_STEP;
					}

					tsize = read( ofds[ 0 ], buffer + buffer_pos, READ_STEP );
					if( tsize <= 0 )
					{
						// end of data, set ending of the string
						buffer[ buffer_pos ] = 0;
						break;
					}

					// succeed reading, move on to next block
					buffer_pos += tsize;
				}
			}

			// cleanup if failed
			close( ofds[ 0 ] );
			if( waitpid( cid, &result, 0 ) < 0 )result = -1;
			if( outbuf )
				*outbuf = buffer;
			else if( buffer )
				free( buffer );
			if( outlen )*outlen = buffer_pos;
		}
	}

	return result;

	#undef REALLOC_STEP
	#undef WRITE_STEP
	#undef READ_STEP
}

void get_shell_output( void )
{
	PBYTE	outbuf = NULL;
	int		outlen;
	int result;

	result = shell_output( &outbuf, &outlen, NULL, 0, "ls", "-a", NULL );

	if( outbuf && result == 0 )
	{
		printf("buf: %s len: %d-%d\n", (char *)outbuf, outlen, strlen( ( char *)outbuf ));
	}
}

void print_detail( Students *param )
{
	if( !param )
	{
		printf("invalied....\n");
	}
	else
	{
		printf("name :%s age: %d\n", param->name, param->age);
	}
	
}

void print_students_list( Students *data, int len )
{
	while( len-- )
	{
		print_detail( data );
		data++;
	}
}

void init_students( int len )
{
	int i;
	Students *data = NULL;
	if( !data && !( data = malloc( ( len) * sizeof( Students ) ) ) )
	{
		printf("Malloc failed!\n");
	}

	char *buf[ 5 ] = {"Stu1", "Stu2", "Stu3", "Stu4", "Stu5"};
	for( i = 0; i < len; i++ )
	{
		ZERO_DATA( data[ i ] );		
		
		//方式一
		char str[100];
		snprintf( str, sizeof( str ), "Stu-%d", i );
		data[i].name = str; //这是错误的赋值

		//方式二
		data[i].name = buf[i];

		data[i].age = i + 10;
	}

	print_students_list( data, len );
}


int main( int argc, char *argv[] )
{
	install_fault_handler( );

	//get_passwd( );
	//get_param( argc, argv );
	//get_shell_output(  );

	//init_students( 5 );

	return 0;
}

