// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <signal.h>
// #include <execinfo.h>
// #include <pwd.h>
// #include <shadow.h>
// #include <sys/time.h>
#include <netdb.h>

#include "common.h"

#define ZERO_DATA( x )	memset( &( x ), 0, sizeof( x ) )
#define GET_TIMESTAMP( )	time( NULL )

#define BLACK_LIST_FILE				"/home/learn/black_list.txt"
#define ORI_BLACK_LIST_FILE			"/home/learn/c/unix_program/black_list.lst"
#define BLACK_LIST_URL				"https://nssa.zxseccloud.com:28443/zxsas/static/template/black.txt"

#define AVRO_FILE					"/home/learn/c/unix_program/avro-c.tar.gz"
#define KAFKA_FILE					"/home/learn/c/unix_program/librdkafka-master.zip"


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

void detach_console( void )
{
	int fd;

	// close all open handles
	for( fd = 0; fd < 65536; fd ++ )close( fd );

	// reopen stdin, stdout and stderr
	fd = open( "/dev/null", O_RDWR );
	dup( fd );
	dup( fd );
	dup( fd );
	chdir( "/" );
}

int shell_execute( PCSTR path, ... )
{
	#define EXEC_FLAGS_WAIT		1
	#define EXEC_FLAGS_DETACH	2

	int		result = -1, cid;
	DWORD	flags = EXEC_FLAGS_WAIT | EXEC_FLAGS_DETACH;

	// if the flags specified in the command line
	if( *path == '&' )
	{
		flags &= ~EXEC_FLAGS_WAIT;
		path ++;
	}
	if( *path == '@' )
	{
		flags &= ~EXEC_FLAGS_DETACH;
		path ++;
	}

	// some cgi server do not set this, and we need it to
	// execute the tools. note the 'overwrite' is '0'
	setenv( "PATH", "/bin:/sbin:/usr/bin:/usr/sbin", 0 );
	// must flush the cached data. otherwise the output before
	// the fork would be flushed by both parent and the child
	fflush( NULL );

	cid = fork( );
	if( cid == 0 )
	{
		va_list	ap;
		int		argc;
		PCSTR	argv[ MAX_EXECUTE_ARGS_SIZE ];

		// the child should not output into the parent's console
		if( flags & EXEC_FLAGS_DETACH )detach_console( );

		// parse and generate the argument array
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

		// and exit
		exit( result );
	}
	else if( cid > 0 )
	{
		// parent waits for the result
		if( flags & EXEC_FLAGS_WAIT )
		{
			if( waitpid( cid, &result, 0 ) < 0 )result = -1;
		}
		else result = 0;
	}
	
	return result;
}

void print( void *data )
{
	PCSTR buf = ( PCSTR )data;
	printf("buf: %s\n", buf);
}

void get_shell_output( void )
{
	PBYTE	outbuf = NULL;
	int		outlen;
	int result;

	#define URL	"http://192.168.33.251:4567/zxsas/static/template/black.txt"

	//result = shell_output( &outbuf, &outlen, NULL, 0, "ls", "-a", NULL );
	//result = shell_output( &outbuf, &outlen, NULL, 0, "wget", "-O", "black_list.txt", URL, NULL );	
	result = shell_output( &outbuf, &outlen, NULL, 0, "uname", "-a", NULL );


	if( outbuf && outlen > 0 )
	{
		print( outbuf );
	}

	if( outbuf )
	{
		printf("*****\n");
		free( outbuf );
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

/**
 *struct tm
 {
	int tm_sec; 代表目前秒数，正常范围为0-59，但允许至61秒 
	int tm_min; 代表目前分数，范围0-59
	int tm_hour; 从午夜算起的时数，范围为0-23
	int tm_mday; 目前月份的日数，范围1-31
	int tm_mon; 代表目前月份，从一月算起，范围从0-11
	int tm_year; 从1900 年算起至今的年数,其值等于实际年份减去1900
	int tm_wday; 一星期的日数，从星期一算起，范围为0-6 ,其中0代表星期天，1代表星期一，以此类推
	int tm_yday; 从今年1月1日算起至今的天数，范围为0-365
	int tm_isdst; 日光节约时间的旗标,实行夏令时的时候，tm_isdst为正,不实行夏令时的时候，
	                tm_isdst为0，不了解情况时，tm_isdst()为负。
 }
 */
void get_time_format( void )
{
	#define FIELD_OFFSET( type, field )		( ( UINT )( UINT )&( ( ( type * )0 )->field ) ) //获取该变量在内存中的地址
	#define TIME_VALUE( tv, idx )			( *( int * )( ( PBYTE )( tv ) + ( idx ) ) )

	time_t ts = GET_TIMESTAMP( );	
	struct tm tm = *localtime( &ts );

	printf("%02x\n", FIELD_OFFSET( struct tm, tm_yday));

	printf("sec_1: %d\n", tm.tm_yday);
	printf("sec_2 : %d\n", TIME_VALUE( &tm, FIELD_OFFSET( struct tm, tm_min)));
	#undef FIELD_OFFSET
	#undef TIME_VALUE

	return;
}

static inline void download_black_list( void )
{
	unlink( BLACK_LIST_FILE );
	shell_execute("curl", "-k", "-s", "-o", BLACK_LIST_FILE, BLACK_LIST_URL, NULL );
	shell_execute( "mv", BLACK_LIST_FILE, "/home/learn/black.txt", NULL );
	printf("success\n");
}

void rename_file( void )
{
	struct stat 	stdata;
	char temp[256];

	download_black_list( );

	if( ( stat( BLACK_LIST_FILE, &stdata ) == 0 ) && ( stdata.st_size > 0 ) )
	{
		snprintf( temp, sizeof( temp ), "%s.ori", ORI_BLACK_LIST_FILE );
		unlink( temp );
		rename( ORI_BLACK_LIST_FILE, temp );

		shell_execute( "cp", BLACK_LIST_FILE, ORI_BLACK_LIST_FILE, NULL );
	}
	else
	{
		printf("Failed download black list file!\n");
	}
}

void check_file_exists( void )
{
	/* Method-1 */
	// FILE *fp;
	
	// if( ( fp = fopen( AVRO_FILE, "r" ) ) )
	// {
	// 	printf("success\n");
	// }
	// else
	// {
	// 	printf("error\n");
	// }
	// close( fp );

	/* Method-2 */
	// if( access( AVRO_FILE, F_OK ) == 0 )
	// {
	// 	printf("success\n");
	// }
	// else
	// {
	// 	printf("error: %s\n", strerror( errno ));
	// }

	/* Method-3 */
	// struct stat  stdata;

	// if( ( stat( KAFKA_FILE, &stdata ) == 0 ) && ( stdata.st_size > 0 ) )
	// {
	// 	printf("success\n");
	// }
	// else
	// {
	// 	printf("error: %s\n", strerror( errno ));
	// }
}

// static const char *product = "product";
// static const char *product_1 = "test";
#define test( type )	\
	do 						\
	{						\
		if( type == product )			\
		{								\
			printf("buf: %s\n", buf);	\
		}								\
		else							\
		{								\
			printf("buf: %s\n", sbuf);	\
			buf++;						\
		}								\
	}while(0);							

void wait_childs( void )
{
	pid_t	pid;
	int		stat;

	while( ( pid = waitpid( -1, &stat, WNOHANG ) ) > 0 );	

}


int main( int argc, char *argv[] )
{
	install_fault_handler( );

	//get_passwd( );
	//get_param( argc, argv );
	//get_shell_output(  );

	//init_students( 5 );
	//get_time_format( );
	//rename_file( );	
	//download_black_list( );
	//check_file_exists( );
	// const char *buf = "acnmoainca";
	// const char *sbuf = "kioasncna";
	// test( product );

	shell_execute( "ls", "-l", NULL );


	return 0;
}

