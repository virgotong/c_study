#include <unistd.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h> 
#include <time.h>

#include "code.h"

#define DEBUG

#define TYPE1	0X0001
#define TYPE2	0X0002

#define ZERO_DATA( x )		memset( &( x ), 0, sizeof( x ) )

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

	int i;
	int len = 10;
	stu2 *data_2 = NULL;
	if( !data_2 && !( data_2 = malloc( ( len) * sizeof( stu2 ) ) ) )
	{
		printf("Create data_2 array failed!\n");
	}

	for( i = 0; i < len; i++ )
	{
		strcpy( data_2[ i ].name, "data_2");
		data_2[ i ].age = i + 10;
		data_2[ i ].score = i + 20;
		data_2[ i ].height = i + 30;
	}

	test_struct( data_2, len );

	if( data_2 )
	{
		free( data_2 );
	}




	return 0;
}

