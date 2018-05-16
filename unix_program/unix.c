#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#include <pwd.h>
#include <shadow.h>

#define MAX_TRACE_SIZE	32

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

int main( int argc, char *argv[] )
{
	install_fault_handler( );

	//get_passwd( );
	get_param( argc, argv );

	return 0;
}

