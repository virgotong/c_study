#include <unistd.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h> 
#include <time.h>
#include <sys/wait.h>
#include <stdarg.h>

#include "code.h"

#define DEBUG

#define TYPE1	0X0001
#define TYPE2	0X0002

typedef unsigned int								UINT, *PUINT;
typedef unsigned char								BYTE, *PBYTE;


#define ZERO_DATA( x )		memset( &( x ), 0, sizeof( x ) )


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
	LIST_ENTRY entry;
	int input_bps;
	int output_bps;
} flowdata;

static LIST_ENTRY 	G_flowdata_list = LIST_ENTRY_INIT( G_flowdata_list );

#define FIELD_OFFSET( type, field )					( ( UINT )( UINT )&( ( ( type * )0 )->field ) )
#define GET_LIST_OBJECT( pe, type, member )			( ( type * )( ( PBYTE )( pe ) - FIELD_OFFSET( type, member ) ) )

typedef struct 
{
	int age;
	char *name;
}test1;

typedef struct
{
	int num;
	char *type;
}test2;

typedef struct 
{
	char name[256];
	int age;
	int score;
	int height;
} stu1;


typedef struct 
{
	char name[256];
	int age;
	int score;
	int height;
} stu2;

typedef struct 
{
	int input_bps;
	int output_bps;
	time_t ts;
	char *comments;	
} rate_data;

void test_dup( void )
{
	int oldfd, newfd;
	char *oldbuf = "old buffering\n";
	char *newbuf = "new buffering\n";

	oldfd = open( "test.txt", O_CREAT|O_RDWR, 0644 );

	printf("oldfd: %d\n", oldfd);

	if( oldfd == -1 )
	{
		printf("Failed open file!\n");
		exit( -1 );
	}

	newfd = dup( oldfd );

	printf("newfd: %d\n", newfd);

	write( oldfd, oldbuf, strlen( oldbuf ) );
	close( oldfd );

	write( newfd, newbuf, strlen( newbuf ) );

	if( close( newfd ) == -1 )
	{
		printf("close failed!\n");
		exit( -1 );
	}	
}

void test_dup2( void )
{
	int fd;  
    int refd;  
    char *buf="Advanced Programming!\n";  
    fd = open("test.txt",O_RDWR|O_CREAT,0644);  
    if( fd==-1 )  
    {  
            printf("open file error\n");  
            exit(-1);  
    }  
    refd = dup2(fd,fileno(stdout));  
    if( refd==-1 )  
    {  
            printf("redirect standard out error\n");  
            exit(-1);  
    }  

    //写数据，本应该写入到stdout的信息重定向而写入到目标文件中（test.txt）  
    printf("dup2的返回值：%d\n",refd);  
    write(fileno(stdout),buf,strlen(buf));  
    close(fd);  
    exit(0);  
}

int test_stat( int argc, char *argv[] )
{
	int i;
	struct stat buf;
	char *ptr;

	for( i = 0; i < argc; i++ )
	{
		if( lstat( argv[i], &buf ) < 0 )
		{
			printf("lstat error!\n");
			continue;
		}

		if( S_ISREG( buf.st_mode ) )
		{
			ptr = "regular";
		}
		else if( S_ISDIR( buf.st_mode ) )
		{
			ptr = "directory";
		}
		else if( S_ISCHR( buf.st_mode ) )
		{
			ptr = "character special";
		}
		else if( S_ISBLK( buf.st_mode ) )
		{
			ptr = "block special";
		}
		else if( S_ISFIFO( buf.st_mode ) )
		{
			ptr = "fifo";
		}
		else if( S_ISLNK( buf.st_mode ) )
		{
			ptr = "link";			
		}
		else if( S_ISSOCK( buf.st_mode ) )
		{
			ptr = "block";
		}
		else
		{
			ptr = "unknown!";
		}
		printf("%s: %s\n", argv[i], ptr);
	}

	return 0;
}

void test_void_ptr( void *ptr, int len, int type )
{
	int i;
	switch( type )
	{
		case TYPE1:
		{
			test1 *data_1 = ( test1 *)ptr;
			for( i = 0; i < len; i++ )
			{
				printf("age: %d name: %s\n", data_1[i].age, data_1[i].name);
			}
			break;
		}

		case TYPE2:
		{
			test2 *data_2 = ( test2 *)ptr;
			for( i = 0; i < len; i++ )
			{
				printf("num: %d type: %s\n", data_2[i].num, data_2[i].type);
			}
			break;
		}

		default:
			break;
	}

	printf("end of func!\n");
}

void init_data( int len, int type )
{
	int i;
	switch( type )
	{
		case TYPE1:
		{
			test1 *ARRAY_1 = NULL;
			if( !ARRAY_1 && !( ARRAY_1 = malloc( ( len ) *  sizeof( test1 ) ) ) )
			{
				printf("declare ARRAY_1 failed! \n");
				return;
			}

			for( i = 0; i < len; i++ )
			{
				ARRAY_1[ i ].age = i;
				ARRAY_1[ i ].name = "test1";
			}

			test_void_ptr( ARRAY_1, len, type );

			if( ARRAY_1 )	free( ARRAY_1 );

			break;
		}

		case TYPE2:
		{
			test2 *ARRAY_2 = NULL;
			if( !ARRAY_2 && !( ARRAY_2 = malloc( ( len ) *  sizeof( test2 ) ) ) )
			{
				printf("declare ARRAY_2 failed! \n");
				return;
			}

			for( i = 0; i < len; i++ )
			{
				ARRAY_2[ i ].num = i+10;
				ARRAY_2[ i ].type = "test2";
			}

			test_void_ptr( ARRAY_2, len, type );

			if( ARRAY_2 ) free( ARRAY_2 );

			break;
		}

		default:
			break;
	}	
}

void fork_process_send_data( void *ptr, int step )
{
	int pid;
	rate_data *data = ( rate_data *)ptr;
	//rate_data *data_ptr = data;

	int cnt = 0;

	if( ( pid = fork( ) ) == 0 )
	{
		printf("子进程: %d\n", getpid( ));

		while( cnt < step )
		{			
			printf("input_bps: %d output_bps: %d ts: %lu, comments: %s\n", data->input_bps, data->output_bps, data->ts, data->comments);
			data++;
			cnt++;	
			printf("cnt: %d\n", cnt);
		}
		exit( 0 );
	}	
}

void init_process( void *ptr, int len )
{
	int step = 10;
	rate_data *data_ptr = (rate_data *)ptr;

	int i;
	for( i = 0; i < len; i += step )
	{
		if( ( i + step ) > len )
		{
			step = len - i;
		}

		printf("主进程: %d\n", getpid( ));
		printf("i: %d step: %d\n", i, i + step);

		fork_process_send_data( data_ptr + i, step);
	}

	sleep(10);
	
}

void init_rate_data( int len )
{
	rate_data *RATE_DATA = NULL;
	if( !RATE_DATA && !( RATE_DATA = malloc( ( len ) * sizeof( rate_data ) ) ) )
	{
		printf("declare rate data array failed!\n");
	}

	int i;
	for( i = 0; i < len; i++ )
	{
		ZERO_DATA( RATE_DATA[ i ] );
		RATE_DATA[ i ].input_bps = i + 1;
		RATE_DATA[ i ].output_bps = i + 10;
		RATE_DATA[ i ].ts = time( NULL );
		RATE_DATA[ i ].comments = "test rate data";
	}

	init_process( RATE_DATA, len );

	if( RATE_DATA )
	{
		free( RATE_DATA );
	}
}

test1* get_data( test1 *data, int len )
{
	int i;
	test1 *DATA = data;
	
	if( !DATA && !( DATA = malloc( ( len ) * sizeof( test1 ) ) ) )
	{
		printf("error!\n");
	}

	for( i = 0; i < len; i++ )
	{
		ZERO_DATA( DATA[ i ] );

		DATA[ i ].name = "test1";
		DATA[ i ].age = i + 10;
	}

	return DATA;
}

int test_step( int len )
{
	
	int process_cnt = 10;
	int step;
	int remainder;	

	if( ( len % process_cnt ) == 0 )
	{
		step = len / process_cnt;
		printf("每组: %d\n", step);
	}
	else
	{
		step = len / process_cnt;
		remainder = len % process_cnt;
		printf("step: %d last: %d + %d\n", step, step, remainder );
	}

	return 0;
}

int test_access( const char *file_name )
{
	if( access( file_name, F_OK ) == 0 )
	{
		printf("文件存在！\n");
	}
	else
	{
		printf("文件不存在！\n");
	}

	return 0;
}

int is_non_symbol( char c )
{
	if( c == '\0')	return 1;

	int c_int = (int)c;
    int symbol = (c_int >= 48 && c_int <= 57) || (c_int >= 65 && c_int <= 90) || (c_int >= 97 && c_int <= 122);
    return symbol;
}

char *url_encode(const char *input)
{
    int end = strlen(input);
    size_t final_size = (end * 3) + 1;
    char *working = malloc(final_size * sizeof(char)),*output = working;

    while(*input)
    {
        const char c = *input;
        if(c < 0)
        {
            input++;
        }
        else if( is_non_symbol( c ) ) 
        {            
            *working++ = *input++;
        }
        else
        {
            char encoded[4] = {0};
            snprintf(encoded, 4, "%%%02x", c);

            *working++ = encoded[0];
            *working++ = encoded[1];
            *working++ = encoded[2];
            input++;
        }
    }

    *working = 0; //null term
    return output;
}

void test_struct( void *data, int len )
{

	int i = 0;	
	stu1 *data_1 = ( stu1 *)data;

	while( i < len )
	{
		printf("name: %s age: %d score: %d\n", data_1->name, data_1->age, data_1->score);
		data_1++;
		i++;
	}

	return;
}

int base64_encode( unsigned char *buf, int len, char *outbuf, int outsize )
{
	int i, outlen;
	unsigned char c1, c2, c3;
	static const unsigned char base64_encode_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	if( ( len + 2 ) * 8 / 6  >= outsize )		
		len = ( outsize -1 ) * 6 / 8;

	printf("len: %d\n", len);
	for( i = 0, outlen = 0; i <= len - 3; i += 3 )
	{
		c1 = buf[ i ];
		c2 = buf[ i + 1 ];
		c3 = buf[ i + 2 ];

		outbuf[ outlen ++ ] = base64_encode_chars[ c1 >> 2 ];
		outbuf[ outlen ++ ] = base64_encode_chars[ ( ( c1 & 0x3 ) << 4 ) | ( ( c2 & 0xf0 ) >> 4 ) ];
		outbuf[ outlen ++ ] = base64_encode_chars[ ( ( c2 & 0xf ) << 2 ) | ( ( c3 & 0xc0 ) >> 6 ) ];
		outbuf[ outlen ++ ] = base64_encode_chars[ c3 & 0x3f ];
	}

	printf("i: %d\n", i);
	if( i < len )
	{
		c1 = buf[ i ++ ];
		if( i < len )
		{
			c2 = buf[ i ++ ];
			outbuf[ outlen ++ ] = base64_encode_chars[ c1 >> 2 ];
			outbuf[ outlen ++ ] = base64_encode_chars[ ( ( c1 & 0x3 ) << 4 ) | ( ( c2 & 0xf0 ) >> 4 ) ];
			outbuf[ outlen ++ ] = base64_encode_chars[ ( ( c2 & 0xf ) << 2 ) ];
			outbuf[ outlen ++ ] = '=';
		}
		else
		{
			outbuf[ outlen ++ ] = base64_encode_chars[ c1 >> 2 ];
			outbuf[ outlen ++ ] = base64_encode_chars[ ( ( c1 & 0x3 ) << 4 ) ];
			outbuf[ outlen ++ ] = '=';
			outbuf[ outlen ++ ] = '=';
		}
	}

	outbuf[ outlen ] = 0;
	return outlen;
	
}


void test_pipe( void )
{
	int fd[2];
    char buf[256];
    int cid;
   
    if( pipe(fd)<0 )
    {
            perror("failed to pipe");
            exit(1);
    }
    
    cid = fork( );
    if( cid == 0 )
    {
    	close( fd[1] );       
        while( 1 )
        {
        	printf("This is child pid %d read from pipe at %ld\n", getpid( ), time( NULL ));
        	int num = read( fd[0], buf, sizeof( buf ) );
        	printf("child read result is %s %d %d\n", buf, strlen( buf ), num );	
        }
        
    }

    for( ; ; sleep( 10 ) )
    {
    	char *sbuf = "test";
    	close(fd[0]);
    	int i;
    	for( i = 0; i < 5; i++ )
    	{
    		write( fd[1], sbuf, strlen( sbuf ) );
    		sleep(1);    		  	
    	}
    	
    }
    
}


//test list
void init_list( void )
{
	int i;
	for( i = 0; i < 10; i++ )
	{
		flowdata data, *pdata;

		ZERO_DATA( data );
		data.input_bps = 100 + i;
		data.output_bps = 1000 + i;

		pdata = ( flowdata *)malloc( sizeof( *pdata ) );
		*pdata = data;

		InsertTailList( &G_flowdata_list, &pdata->entry );
	}

	return;
}

void print_list( void )
{
	PLIST_ENTRY pe;
	flowdata *pdata;

	for( pe = G_flowdata_list.Flink; pe != &G_flowdata_list; )
	{
		pdata = GET_LIST_OBJECT( pe, flowdata, entry );
		pe = pe->Flink;

		printf( "input_bps: %d output_bps: %d\n", pdata->input_bps, pdata->output_bps );

		RemoveEntryList( &pdata->entry );
		free( pdata );
	}

	return;
}

void common_update_file( void )
{
	int cid;
	cid = fork( );
	if( cid == 0 )
	{
		char *fname = "black_list.txt";
		struct stat stdata;
		struct tm *accesstime;
		struct tm *modifytime;
		struct tm *changetime;

		if( ( stat( fname, &stdata ) == 0 ) && ( stdata.st_size > 0 ) )
		{
			accesstime = localtime( &( stdata.st_atime ) );
			modifytime = localtime( &( stdata.st_mtime ) );
			changetime = localtime( &( stdata.st_ctime ) );

			printf("accesstime: %d::%d::%d\n", accesstime->tm_hour, accesstime->tm_min, accesstime->tm_sec);
			printf("modifytime: %d::%d::%d\n", modifytime->tm_hour, modifytime->tm_min, modifytime->tm_sec);
			printf("changetime: %d::%d::%d\n", changetime->tm_hour, changetime->tm_min, changetime->tm_sec);
		}
		else
		{
			printf("File not exists!\n");
		}	
	}
}

void wait_childs( void )
{
	pid_t	pid;
	int		stat;

	while( ( pid = waitpid( -1, &stat, WNOHANG ) ) > 0 );
}

typedef const char *	PCSTR;

static inline int my_is_digit( int c )
{
	return	( c >= '0' && c <= '9' );
}

void print_safe_page_string( char *fmt, char *str )
{
	char			*bufptr, *pos, *trans;
	int				bufsize, outpos, len;
	static char	buffer[4096];

	// first, use static buffer
	bufptr = buffer;
	bufsize = sizeof( buffer );

	for( ; ; )
	{
		// check the string for special html characters
		for( outpos = 0, pos = str; *pos; pos ++ )
		{
			trans = NULL;
			if( ( BYTE )*pos < 0x20 )
			{
				if( *pos != '\r' &&
					*pos != '\n' &&
					*pos != '\t' )
					trans = ".";
			}
			else if( *pos == '<' )
				trans = "&lt;";
			else if( *pos == '>' )
				trans = "&gt;";
			else if( *pos == '&' )
				trans = "&amp;";
			else if( *pos == '\"' )
				trans = "&quot;";
			else if( *pos == '\'' )
				trans = "&apos;";

			if( trans )
			{
				// need to transform the character to html notations
				len = strlen( trans );
				if( outpos + len >= bufsize )break;
				memcpy( bufptr + outpos, trans, len );
				outpos += len;
			}
			else
			{
				// default copy
				if( outpos + 1 >= bufsize )break;
				bufptr[ outpos ++ ] = *pos;
			}
		}

		if( ! *pos )break;
		if( bufptr != buffer )free( bufptr );
		bufsize += strlen( pos ) * 8;	// make big enough
		bufptr = ( char * )malloc( bufsize );
		if( ! bufptr )return;
	}

	// finally, output the transformed string
	bufptr[ outpos ] = 0;
	printf( fmt, bufptr );
	if( bufptr != buffer )free( bufptr );
}

void my_vprintf( const char *fmt, va_list args )
{
	int		len, offset = 1;
	PCSTR	pos, last = fmt;
	char	newfmt[4096];
	va_list	lastargs;
	va_copy( lastargs, args );

	for( pos = fmt; *pos; )
		if( *pos == '%' )
		{
			offset = 1;

			// strip flags ...
			switch( pos[ offset ] )
			{
				case '-' :
				case '+' :
				case ' ' :
				case '#' :
				case '0' :

					offset ++;
					break;
			}

			// strip field width
			if( pos[ offset ] == '*' )
				va_arg( args, int );
			else
				while( my_is_digit( pos[ offset ] ) )offset ++;

			// strip precision ...
			if( pos[ offset ] == '.' )
			{
				offset ++;
				while( my_is_digit( pos[ offset ] ) )offset ++;
			}

			// strip the qualifier
			switch( pos[ offset ] )
			{
				case 'h' :
				case 'l' :

					if( pos[ offset ] == pos[ offset + 1 ] )offset ++;
					offset ++;
					break;

				case 'L' :
			    case 'Z' :
			    case 'z' :
			    case 't' :

					offset ++;
					break;
			}

			// get the final type
			switch( pos[ offset ++ ] )
			{				
				case 'd' :
				case 'i' :

					// int agrument
					va_arg( args, int );
					break;

				case 'o' :
				case 'u' :
				case 'x' :
				case 'X' :

					// unsigned int argument					
					printf("...\n");
					va_arg( args, unsigned int );
					break;

				case 'f' :
				case 'F' :

					// float argument
					va_arg( args, double );	// promote 'float' to 'double'
					break;

				case 'a' :
				case 'A' :
				case 'e' :
				case 'E' :
				case 'g' :
				case 'G' :

					// double argument
					va_arg( args, double );
					break;

				case 'p' :

					// pointer argument
					va_arg( args, void * );
					break;

				case 'c' :

					// char argument
					va_arg( args, int );	// promote 'char' to 'int'
					break;

				case 'S' :

					// format output previous arguments
					len = ( int )( pos + offset - last );
					memcpy( newfmt, last, len );
					newfmt[ len - 1 ] = 's';
					newfmt[ len ] = 0;
					vprintf( newfmt, lastargs );
					va_arg( args, char * );

					last = pos + offset;
					va_copy( lastargs, args );
					break;

				case 's' :

					// string argument
					// this is what we interest in

					// format output previous arguments
					printf("string go here!\n");				
					len = ( int )( pos - last );
					memcpy( newfmt, last, len );
					newfmt[ len ] = 0;
					vprintf( newfmt, lastargs );

					// VERIFY and output current string
					memcpy( newfmt, pos, offset );
					newfmt[ offset ] = 0;
					print_safe_page_string( newfmt, va_arg( args, char * ) );

					last = pos + offset;
					va_copy( lastargs, args );
					break;
			}

			pos += offset;
		}
		else pos ++;

	// format output the remaining arguments	
	if( *last )vprintf( last, lastargs );

	//debug code add by dongsixing
	FILE *fp;
	const char *file = "/var/log/print.debug";
	fp = fopen( file, "a" );	
	if( *last ) vfprintf( fp, last, lastargs );
	fclose( fp );
}

void print_data_fotmat( const char *format, ... )
{
	va_list 	args;
	va_start( args, format );
	my_vprintf( format, args );
	va_end( args );
}

int main( int argc, char *argv[] )
{
	// test_dup( );
	// test_dup2( );

	//test_stat( argc, argv );

	// init_data( 10, TYPE1 );

	// sleep( 5 );

	// init_data( 10, TYPE2 );

	 // init_rate_data( 5 );
	 // sleep(10);

	// test1 *data = NULL;
	// data = get_data( data, 10 );
	// test_void_ptr( data, 10, TYPE1 );
	// int len = atoi( argv[1] );
	// test_step( len );

	// const char *file_name = "/tmp/test.txt";
	// test_access( file_name );

	// char *url = "bIdJYi##XFBXDzK85vTe4";
	// char *encodeUrl = url_encode( url );

	// printf("encodeUrl: %s\n", encodeUrl);

	// if( encodeUrl )
	// {
	// 	free( encodeUrl );
	// }

	// #ifdef DEBUG
	// 	printf("debug!\n");
	// #else
	// 	printf("no debug!\n");

	// #endif

	// int i;
	// int len = 10;
	// stu2 *data_2 = NULL;
	// if( !data_2 && !( data_2 = malloc( ( len) * sizeof( stu2 ) ) ) )
	// {
	// 	printf("Create data_2 array failed!\n");
	// }

	// for( i = 0; i < len; i++ )
	// {
	// 	strcpy( data_2[ i ].name, "data_2");
	// 	data_2[ i ].age = i + 10;
	// 	data_2[ i ].score = i + 20;
	// 	data_2[ i ].height = i + 30;
	// }

	// printf("--pid--: %d\n", getpid());
	// sleep( 10 );

	// int cid;
	// cid = fork( );
	// if( cid == 0 )
	// {
	// 	test_struct( data_2, len );
	// 	printf("pid: %d\n", getpid( ));
	// }


	//test_struct( data_2, len );

	// if( data_2 )
	// {				
	// 	free( data_2 );
	// }

	// const char *buf = "php1";
	// char sbuf[4096];
	// int encode_len = base64_encode( (unsigned char *)buf, strlen( buf ), sbuf, sizeof( sbuf ) );
	// printf("sbuf: %s encode_len: %d\n", sbuf, encode_len);

	
	//test_pipe( );

	// init_list( );
	// print_list( );


	//common_update_file( );

	//test print data order by fotmat
	
	unsigned int a = 123;

	print_data_fotmat( "%u", a );
	print_data_fotmat( "%lu", 456 );
	print_data_fotmat( "%s", "ajcasjc");
	


	

	return 0;
}
