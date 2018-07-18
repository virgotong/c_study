#include <stdio.h>
#include <pcap.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define FIN  0x01
#define SYN  0x02
#define RST  0x04
#define PUSH 0x08
#define ACK  0x10
#define URG  0x20
#define ECE  0x40
#define CWR  0x80

//eth header
typedef struct {
    u_char DestMac[6];
    u_char SrcMac[6];
    u_char Etype[2];
}ETHHEADER;

//ip header
typedef struct
{  
    unsigned char h_verlen;//4位首部长度，4位IP版本号  
    unsigned char tos;//8位服务类型TOS  
    unsigned short tatal_len;//16位总长度  
    unsigned short ident;//16位标示  
    unsigned short frag_and_flags;//偏移量和3位标志位  
    unsigned char ttl;//8位生存时间TTL  
    unsigned char proto;//8位协议（TCP,UDP或其他）  
    unsigned short checksum;//16位IP首部检验和  
    unsigned int sourceIP;//32位源IP地址  
    unsigned int destIP;//32位目的IP地址  
}IPHEADER;

//tcp header
typedef struct
{
    unsigned short sport;//16位源端口  
    unsigned short dport;//16位目的端口  
    unsigned int seq;//32位序列号  
    unsigned int ack;//32位确认号  
    unsigned char lenres;//4位首部长度/6位保留字  
    unsigned char flag;//6位标志位  
    unsigned short win;//16位窗口大小  
    unsigned short sum;//16位检验和  
    unsigned short urp;//16位紧急数据偏移量  
}TCPHEADER, *PTCPHEADER;

//协议映射表
char *Proto[]={
    "Reserved","ICMP","IGMP","GGP","IP","ST","TCP"
};

void pcap_packet_callback( u_char *user, const struct pcap_pkthdr *header, const u_char *pkt_data )
{   
    if( header->len < sizeof( ETHHEADER ) ) return;

    printf("-------------Begin Analysis-----------------\n");    
    printf("Packet length is: %d\n", header->len);    

    //get ip tcp header    
    IPHEADER  *ip_header   = ( IPHEADER *)( pkt_data + sizeof( ETHHEADER ) );
    TCPHEADER *tcp_header  = ( TCPHEADER *)( pkt_data+ sizeof( ETHHEADER ) + sizeof( IPHEADER ) );

    //TCP only
    if( ip_header->proto != 6 )
    {
        printf("Not tcp packet: %d\n", ip_header->proto);
        return;
    }
    else
    {
        //print ip and port
        struct sockaddr_in saSource, saDest;
        saSource.sin_addr.s_addr = ip_header->sourceIP;
        saDest.sin_addr.s_addr   = ip_header->destIP;
        printf("src_ip: %s src_port: %d\n", inet_ntoa( saSource.sin_addr ), ntohs( tcp_header->sport ));
        printf("dst_ip: %s dst_port: %d\n", inet_ntoa( saDest.sin_addr ), ntohs( tcp_header->dport ));
    
        //print tcp_flag
        printf("tcp_flag: 0x%04x\n", tcp_header->flag);

        //print pkt_data
        int i;
        for( i = 0; i < ( int )header->len; ++i )
        {
            printf("%02x ", pkt_data[ i ]);
            if( ( i + 1 ) % 16 == 0 )
            {
                printf("\n");
            }            
        }
        printf("\n\n");
    }
}

int main(int argc, char **argv)
{    
    char *device;
    char errbuf[1024];
    pcap_t *phandle;
    
    bpf_u_int32 ipaddress,ipmask;
    struct bpf_program fcode;
    int datalink;
    
    if((device=pcap_lookupdev(errbuf))==NULL)
    {
        perror(errbuf);
        return 1;
    }
    else
    {
        printf("device: %s\n",device);
    }
    
    phandle=pcap_open_live(device,200,0,500,errbuf);
    if(phandle==NULL)
    {
        perror(errbuf);
        return 1;
    }
    
    if(pcap_lookupnet(device,&ipaddress,&ipmask,errbuf)==-1)
    {
        perror(errbuf);
        return 1;
    }
    else
    {
        char ip[INET_ADDRSTRLEN],mask[INET_ADDRSTRLEN];
        if(inet_ntop(AF_INET,&ipaddress,ip,sizeof(ip))==NULL)
        {
            perror("inet_ntop error");
        }
        else if(inet_ntop(AF_INET,&ipmask,mask,sizeof(mask))==NULL)
        {            
            perror("inet_ntop error");
        }
        
        printf("IP address: %s, Network Mask: %s\n",ip,mask);
    }
    
    int flag=1;
    while(flag)
    {
        //input the design filter
        printf("Input packet Filter: ");
        char filterString[ 1024 ];
        fgets( filterString, 1024, stdin );
    
        if(pcap_compile(phandle,&fcode,filterString,0,ipmask)==-1)
        {
            fprintf(stderr,"pcap_compile: %s,please input again....\n",pcap_geterr(phandle));
        }
        else
        {            
            flag=0;
        }
    }
    
    if(pcap_setfilter(phandle,&fcode)==-1)
    {
        fprintf(stderr,"pcap_setfilter: %s\n",pcap_geterr(phandle));
        return 1;
    }
    
    if((datalink=pcap_datalink(phandle))==-1)
    {
        fprintf(stderr,"pcap_datalink: %s\n",pcap_geterr(phandle));
        return 1;
    }
    
    printf("datalink= %d\n",datalink);

    pcap_loop(phandle,-1,pcap_packet_callback,NULL);
    
    return 0;
}