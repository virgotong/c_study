/*
 *server.c
 *需要开放对应的端口
 */
#include<stdio.h>
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h> 
#include <arpa/inet.h>
#define DEFAULT_PORT 8000
#define MAXLINE 4096

static void fast_connect( void )
{
    int socket_fd, connect_fd;
    struct sockaddr_in  servaddr;
    char buff[ MAXLINE ];
    int size;    

    //init socket
    if( ( socket_fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        printf("Create socket failed due to: %s\n", strerror( errno ));
        exit( 0 );
    }

    //init servaddr
    memset( &servaddr, 0, sizeof( servaddr ) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.28.117");
    servaddr.sin_port = htons( DEFAULT_PORT );

    //bind
    if( bind( socket_fd, ( struct sockaddr *)&servaddr, sizeof( servaddr ) ) < 0 )
    {
        printf("Bind socket failed due to: %s\n", strerror( errno ));
        exit( 0 );        
    }

    //listen
    if( listen( socket_fd, 10 ) < 0 )
    {
        printf("Listen socket failed due to: %s\n", strerror( errno ));
        exit( 0 );
    }

    if( ( connect_fd = accept( socket_fd, ( struct sockaddr *)NULL, NULL ) ) < 0 )
    {
        printf("Accept socket failed due to: %s\n", strerror( errno ));        
    }

    struct sockaddr_in sa;
    socklen_t len;
    len = sizeof( sa );
    if( getpeername( connect_fd, ( struct sockaddr *)&sa, &len ) == 0 )
    {
        printf("client_ip: %s client_port: %d\n", inet_ntoa(sa.sin_addr), ntohs( sa.sin_port));
    }
    else
    {
        printf("getpeername failed!\n");        
    }

    //wait client request
    printf(">>>>>>>>Waiting for client request>>>>>>>>\n");
    while( 1 )
    {
        size = recv( connect_fd, buff, MAXLINE, 0 );
        send( connect_fd, "Connecting success!", 26, 0 );
        buff[ size ] = '\0';       
        printf("Recv buff from client: %s", buff);        
    }
	close( connect_fd );
    close( socket_fd );  
}

int main(int argc, char** argv)  
{  
    fast_connect( );
    return 0;
}  
