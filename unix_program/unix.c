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
#define ARRAY_SIZE( x ) ( sizeof( x) / sizeof( x)[0] )

#define MAX_STRING_LENGTH		0xFF
typedef char 	String[ MAX_STRING_LENGTH + 1 ];

#define MAX_LSTRING_LENGTH		0xFFF
typedef char	LString[ MAX_LSTRING_LENGTH + 1 ];

#define MAIL_SCAN_DIR			"/var/mail_scan/fileoutdir"

typedef struct
{
	//char name[256];
	char *name;
	int age;
} Students, *PStudents;

void __dbgprint( PCSTR fname, PCSTR format, ... )
{
	FILE			*fp;
	va_list			arg;
	struct stat		stbuf;
	struct timeval	tv;
	struct tm		*tmval;
	PSTR			pos, nextpos;
	String			temp;
	LString			buffer;

	// truncate the log file if too large
	if( stat( fname, &stbuf ) >= 0 &&
		stbuf.st_size > 0x100000 )
	{
		sprintf( temp, "%s.0", fname );
		unlink( temp );
		rename( fname, temp );
	}

	// format message to buffer
	va_start( arg, format );
	vsnprintf( buffer, sizeof( buffer ), format, arg );
	va_end( arg );

	// iutput formatted message
	if( ( fp = fopen( fname, "a" ) ) )
	{
		gettimeofday( &tv, NULL );
		tmval = localtime( ( time_t * )&tv.tv_sec );
		sprintf( temp, "%04d-%02d-%02d %02d:%02d:%02d %03d.%03d ( pid %d ) : ",
			tmval->tm_year + 1900, tmval->tm_mon + 1, tmval->tm_mday,
			tmval->tm_hour, tmval->tm_min, tmval->tm_sec,
			( int )tv.tv_usec / 1000, ( int )tv.tv_usec % 1000, getpid( ) );

		for( pos = buffer; pos && *pos; pos = nextpos )
		{
			nextpos = strchr( pos, '\n' );
			if( nextpos )*nextpos ++ = 0;

			fprintf( fp, "%s %s\n", temp, pos );
		}

		fclose( fp );
	}
}

static inline struct tm * my_localtime( time_t *ts )
{
	static struct tm zero_tmval = { 0 };
	struct tm *tmval = localtime( ts );
	if( ! tmval )tmval = &zero_tmval;
	return tmval;
}

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

static inline BOOL my_is_space( int c )
{
	return c > 0 && c <= 0x20;
}

static int trim_spaces( PSTR string )
{
	PSTR pos, pos_str;

	pos = pos_str = string;

	// find the start position of the characters
	while( my_is_space( *pos ) )pos ++;

	// move the characters to the start if needed
	if( pos != pos_str )
		while( *pos )*pos_str ++ = *pos ++;
	else
		while( *pos_str )pos_str ++;

	// cut the spaces at end of the string
	while( pos_str > string && my_is_space( pos_str[ -1 ] ) )pos_str --;
	if( *pos_str )*pos_str = 0;

	return ( int )( pos_str - string );
}

static  UINT vread_strings( PCSTR source, PCSTR format, UINT strsize, ... )
{
	BOOL		greedy = FALSE, empty = TRUE;
	int			skip = 0;
	UINT		pos, count = 0;
	PSTR		target;
	va_list		marker;

	// initialize
	va_start( marker, strsize );

	// check for greedy matching
	if( *format == 'g' && format[ 1 ] == '/' )
	{
		greedy = TRUE;
		format += 2;
	}

	// Read according to the 'format'
	while( *format )
	{
		if( *format == '%' )						// Special token
		{
			do {
				target = va_arg( marker, PSTR );
				if( ! target )break;
				*target = 0;

			} while( skip -- > 0 );
			if( ! target )break;

			format ++;
			if( *format == '\\' )format ++;

			// Read one string into buffer
			while( my_is_space( *source ) )source ++;

			for( pos = 0; pos < strsize && *source; )
			{
				if( *format == ' ' && my_is_space( *source ) )break;
				if( greedy )
				{
					PCSTR	nextfmt;

					for( nextfmt = format, skip = 0; *nextfmt; nextfmt ++ )
						if( *nextfmt == '%' )
						{
							skip ++;
						}
						else
						{
							if( *nextfmt == '\\' )nextfmt ++;
							if( *nextfmt == *source )break;
						}

					if( *nextfmt == *source )format = nextfmt;
				}
				if( *format == *source )break;
				target[ pos ++ ] = *source ++;
			}
			target[ pos ] = 0;
			trim_spaces( target );

			// read enough strings, break out
			if( *target )empty = FALSE;
			count ++;
			continue;
		}
		else if( *format == ' ' )
		{
			while( my_is_space( *source ) )source ++;
			format ++;
			continue;
		}
		else if( *format == '\\' )
		{
			format ++;
		}

		// Match other characters
		if( *source != *format )break;
		source ++;
		format ++;
	}

	va_end( marker );
	return empty ? 0 : count;
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

/* 
	ip.cn
	ipinfo.io
	ifconfig.me
	http://members.3322.org/dyndns/getip
*/
static void  get_public_ip( void )
{
	PBYTE	 outbuf = NULL;
	int 	 outlen;
	#if 0
	shell_output( &outbuf, &outlen, NULL, 0, "curl", "-s", "http://members.3322.org/dyndns/getip", NULL );
	#endif


	shell_output( &outbuf, &outlen, NULL, 0, "curl", "-s", "ifconfig.me/ip", NULL );

	if( outbuf && outlen > 0 )
	{
		printf("public ip: %s len: %d\n", ( char * )outbuf, outlen );
	}
	else
	{
		printf("curl http://members.3322.org/dyndns/getip failed!\n");
	}
}


static char *get_shell_output( char **buf )
{
	PBYTE	outbuf = NULL;
	int	    outlen;
	char *serial = "etaslxcna1q8y821#499as&^5acn";
	static char client_id[128] = "";
	char address[32]= "";

	strcpy( client_id, serial);

	#if 0
	shell_output( &outbuf, &outlen, NULL, 0, "uname", "-a", NULL );
	#endif	

	#if 1
	shell_output( &outbuf, &outlen, NULL, 0, "curl", "-s", "ip.cn", NULL );	
	#endif

	if( outbuf && outlen > 0 )
	{			
		char *pos = NULL;		
		#if 1
		if( ( pos = strstr( ( char *)outbuf, "IP：" ) ) )
		{			
			pos += strlen( "IP：" );
			snprintf( address, sizeof( address ), "--%.*s", 15, pos );				
		}
		#endif		
	}

	printf("address: %s\n", address);
	strcat( client_id, address );

	if( outbuf )
	{
		free( outbuf );
	}

	//strcpy( buf, client_id );
	*buf = client_id;

	return client_id;
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

void *read_students( void *params )
{
	return NULL;
}

void pthread_deal_students( PStudents data )
{
	if( !data )
	{
		printf("No data input...\n");
		return;
	}

	int itr;
	int len = 2;
	pthread_t thread[ len ];

	for( itr = 0; itr < len; itr++ )
	{
		if( pthread_create( &thread[ itr ], NULL, read_students, ( void *)data ) != 0 )
		{
			printf("Failed to create thread[ %d ]\n", itr);
		}
	}

	for( itr = 0; itr < len; itr++ )
	{
		if( pthread_join( thread[ itr ], NULL ) != 0 )
		{
			printf("Failed to finish thread[ %d ]\n", itr);
		}
	}

	return;
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
		#if 0
		char str[100];
		snprintf( str, sizeof( str ), "Stu-%d", i );		
		data[i].name = str; //这是错误的赋值, 这会导致name指针最后仅仅会指向str这片内存区域
		printf("name: %s\n", data[i].name );
		#endif
		//方式二
		data[i].name = buf[i];

		data[i].age = i + 10;
	}

	//print_students_list( data, len );
	
	pthread_deal_students( data );
}

#if 0 
static const char *		G_netdev_patterns[] = { "xgbe%d", "gbe%d", "eth%d" };
char G_netdev_names[ ARRAY_SIZE( G_netdev_patterns ) ][ 32 ][ IFNAMSIZ ];
void load_netdev_info( )
{
	int			itr, index;	
	FILE		*fp;
	char line[ 256 ];
	char str[256];

	if( ( fp = fopen( "/proc/net/dev", "r" ) ) )
	{
		while( ! feof( fp ) )
		{
			if( fgets( line, sizeof( line ), fp ) )
			{				
				char *pos;

				pos = strchr( line, ':' );
				if( pos )*pos = 0;				
				pos = strchr( line, '.' );
				if( pos )*pos = 0;				
				trim_spaces( line );
				#if 0
				for( itr = 0; itr < ARRAY_SIZE( G_netdev_names ); itr ++ )
					if( sscanf( line, G_netdev_patterns[ itr ], &index ) == 1 &&
						index >= 0 && index < ARRAY_SIZE( G_netdev_names[ itr ] ) )
						SAFE_COPY( G_netdev_names[ itr ][ index ], line );
				#endif
			}
		}

		fclose( fp );
	}
}
#endif

static inline const char *conver_to_str( unsigned long long int data )
{
	static char buf[ 256 ];
	printf("before: %s\n", buf);
	
	snprintf( buf, sizeof( buf ), "%llu", data );
	
	printf("after: %s\n", buf);
	return buf;
}

#if 0
//pthread_barrier_t b;

void* task(void* param) {
    // int id = (int) param;
    // printf("before the barrier %d\n", id);
    // pthread_barrier_wait(&b);
    // printf("after the barrier %d\n", id);
    printf("child thread...\n");
    return NULL;
}

void test_pthread( void )
{

	pthread_t thread;
	pthread_create( &thread, NULL, task, NULL );
	sleep( 2 );
	pthread_join( thread, NULL );
}


void test_pthread( void )
{
	int nThread = 5;
    int i;
 
    pthread_t thread[nThread];
    //pthread_barrier_init(&b, 0, nThread);
    for (i = 0; i < nThread; i++)
        pthread_create(&thread[i], 0, task, (void*)i);
    for (i = 0; i < nThread; i++)
        pthread_join(thread[i], 0);
    //pthread_barrier_destroy(&b);

}
#endif

#if 1
//index_dstip_srcip_dstport_srcport_count_time__.eml
static void print_eml_detail( const char *filename )
{
	PCSTR 	start, head, pos;
	String 	segment, srcip, dstip, protocol, time;
	int 	i, name_len, proto, srcport, dstport;

	ZERO_DATA( segment );
	ZERO_DATA( srcip );
	ZERO_DATA( dstip );
	ZERO_DATA( protocol );
	ZERO_DATA( time );
	i = name_len = proto = srcport = dstport = 0;
	start = filename + strlen( MAIL_SCAN_DIR );

	for( head = start + 1, pos = NULL; head <= filename + 512; head = pos +1 )
	{
		name_len = 0;
		pos = strchr( head, '_' );

		printf( "pos: %s\n", pos);

		if( !pos )
		{
			pos = filename + 512;
			name_len = pos - head + 1;
		}
		else
		{
			name_len  = pos - head;
		}

		if( ( name_len <= 0) || ( name_len >= MAX_STRING_LENGTH ) ) break;

		switch( i++ )
		{
			case 0:
				{
					snprintf( segment, MAX_STRING_LENGTH, "%.*s", name_len, head );
					proto = atoi( segment );
				}
				break;

			case 1:
				{
					snprintf( dstip, MAX_STRING_LENGTH, "%.*s", name_len, head );
				}
				break;

			case 2:
				{
					snprintf( srcip, MAX_STRING_LENGTH, "%.*s", name_len, head );	
				}
				break;

			case 3:
				{
					snprintf( segment, MAX_STRING_LENGTH, "%.*s", name_len, head );
					dstport = atoi( segment );
				}
				break;

			case 4:
				{
					snprintf( segment, MAX_STRING_LENGTH, "%.*s", name_len, head );
					srcport = atoi( segment );	
				}
				break;

			case 6:
				{
					snprintf( time, MAX_STRING_LENGTH, "%.*s", name_len, head );
					time_t start_ts = strtoul( time, NULL, 0 );
					strftime( time, sizeof( time ), "%F %T", my_localtime( &start_ts ) );
				}
				break;

			default:
				break;
		}
	}

	switch( proto )
	{
		case 1:
			strcpy( protocol, "smtp" );
			break;

		case 2:
			strcpy( protocol, "pop3" );
			break;

		case 3:
			strcpy( protocol, "imap" );
			break;

		default:
			strcpy( protocol, "unknown" );
			break;
	}

	printf( "protocol: %s srcip: %s dstip: %s srcport: %d dstport: %d time: %s\n", protocol, srcip, dstip, srcport, dstport, time );
}

#endif

int main( int argc, char *argv[] )
{
	install_fault_handler( );

	//get_passwd( );
	//get_param( argc, argv );
	
	#if 0
	char *buf = NULL;
	get_shell_output( &buf );
	printf("buf: %s\n", buf);
	#endif

	#if 0
	//char *client_id = NULL;
	char client_id[128];	
	strcpy( client_id, get_shell_output() );
	//client_id = get_shell_output( );
	printf("client_id: %s\n", client_id);	
	#endif

	//init_students( 5 );
	
	#if 0
	int i;
	for( i = 0; i < 2; i++ )
	{
		fork();
		printf( "*" );
	}
	#endif

	#if 0
	char *str = ( char *)malloc( 10 * sizeof( char ));
	memset( &str, 0, strlen( str ) );

	char *p = str;

	memcpy( str, "test", 4 );

	*str++ = 0; //此时内存之乡的位置已经发生变化，所以后面free的时候会报错	
	printf( "str: %s\n", str );

	if( p )
	{
		free( p );
		printf( "free success!\n" );
	}
	#endif

	#if 0
		PStudents params;
		int index;
		params = ( PStudents )malloc( 10 * sizeof( Students ) );
		
		if( params )
		{
			free( params );
			printf("free success!\n");
		}
	#endif

	#if 0
	//load_netdev_info( );
	#endif		

	#if 0
	int num[ 4 ] = {254, 15644, 56465, 56454 };
	int itr;

	for( itr = 0; itr < 4; itr++ )
	{
		conver_to_str( num[ itr ] );
	}
	#endif

	#if 0
		test_pthread( );
	#endif

	#if 0
		char *str = "^defg$";
		char buf[256];
		unsigned char sig;
		int id;
		sscanf("1:/abc/i", "%d%*2c%[^/]/%c", &id, buf, &sig);
		printf("id: %d buf: %s sig: %c\n", id, buf, sig);
		printf("len: %d\n", strlen( str ) );
	#endif

	#if 0
		#define FLAG_0	0X0080
		#define FLAG_1	0X0001
		#define FLAG_3  0X0002
		unsigned int flag = 0;
		flag |= FLAG_0;
		unsigned int key = 0;
		key = flag & FLAG_0;
		printf("key: %u - 0x%04x\n", key, key);
	#endif

	#if 0
	const char *mail_name = "25_1.1.1.1_2.2.2.2_12365_143_12_1503302098__.eml";
	String fname;
	sprintf( fname, "%s/%s", MAIL_SCAN_DIR, mail_name);
	print_eml_detail( fname );
	#endif

	#if 1
	//get_public_ip( );
	#endif

	#if 0
		char buffer[64] = "EPRT |1|132.235.1.2|6275|";
		char *ip, *sport;
		int port;

		ip = buffer + 8;		
		sport = strchr( ip + 1, '|' );
		if( !sport ) printf("failed\n");

		*sport = 0; // 将 '|'更改成\0
		sport++;
		printf("ip: %s sport: %s\n", ip, sport);
		port = atoi( sport );

		printf("port: %d\n", port );

	#endif

	#if 1
		#define LOG_FILE						"./log.txt"
		#define WRITE_LOG( fmt, ... )			__dbgprint( LOG_FILE, fmt, ## __VA_ARGS__ )

		WORD  code = 0;
		DWORD command = 0;
		DWORD status = 0;		
		BYTE buffer[64] = {0x00, 0x00, 0x34, 0x03, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
		PBYTE ptr = buffer;

		code = *( ptr + 2 );
		//code |= *( ptr + 3 ) << 8; 

		command = *( ptr + 16 );		
		command |= *( ptr + 17 ) << 8 ;
		command |= *( ptr + 18 ) << 16;
		command |= *( ptr + 19 ) << 24;

		status = *( ptr );
		status |= *( ptr + 1 ) << 8;
		status |= *( ptr + 2 );
		status |= *( ptr + 3 ) << 8;

		// printf("code: 0x%04x\n", code);
		// printf( "command: 0x%08x  status: 0x%08x\n", command, status );
		// printf("command: %u\n status: %u\n\n", command, status);
	
		WRITE_LOG( "function: %s line: %d command: 0x%08x status: 0x%08x", __FUNCTION__, __LINE__, command, status );	

		WORD data_offset = 0;
		data_offset = status + 4;

		printf("data_offset: %u\n", data_offset);


		#if 0
		unsigned int i, ip = 0;
		int port;
		const char *p = buffer;

		for( i = 0; i < 4; i++ )
		{
			ip = ip * 256 + atoi( p );
			printf("ip: %u\n", ip);
			p = strchr( p, ',' );
			if( !p ) printf("failed\n");;
			p++;
		}

		port = atoi( p );
		p = strchr( p, ',' );
		if( !p )printf("failed\n");;
		p++;
		port = port * 256 + atoi( p );

		printf("port: %d\n", port);
	#endif

	#endif

	return 0;
}

