/*
 *client.c
 *需要开放对应的端口
 */
#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include <unistd.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>
#include <arpa/inet.h>
  
#define MAXLINE 4096  

static void fast_connect( void )
{
    int sockfd, rec_len;
    char sendline[ MAXLINE ], buff[ MAXLINE ];
    struct sockaddr_in servaddr;

    if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        printf( "Create socket failed due to: %s\n", strerror( errno ));
        exit( 0 );
    }

    memset( &servaddr, 0, sizeof( servaddr ) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons( 8000 );

    if( connect( sockfd, ( struct sockaddr *)&servaddr, sizeof( servaddr ) ) < 0 )
    {
        printf("Connect failed due to: %s\n", strerror( errno ));
        exit( 0 );
    }

    printf("Send msg to server: \n");
    fgets( sendline, MAXLINE, stdin );

    if( send( sockfd, sendline, strlen( sendline ), 0 ) < 0 )
    {
        printf("Send msg failed due to: %s\n", strerror( errno ));
        exit( 0 );
    }

    if( ( rec_len = recv( sockfd, buff, MAXLINE, 0 ) ) < 0 )
    {
        printf("recv failed!\n");
        exit( 0 );
    }

    buff[ rec_len ] = '\0';

    printf("Received msg: %s pid: %d\n", buff, getpid());

    close( sockfd );
}
  
int main(int argc, char** argv)  
{  
    fast_connect( );

    return 0;
}  
