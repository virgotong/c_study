#ifndef _COMMON_H_
#define _COMMON_H_
#include <linux/types.h>
#include <linux/tcp.h>
#include <linux/sockios.h>
#include <linux/tiocl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <execinfo.h>
#include <net/route.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <syslog.h>
#include <dirent.h>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <time.h>
#include <fnmatch.h>
#include <wchar.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/tcp.h>
#include <linux/sockios.h>
#include <linux/tiocl.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <errno.h>

#include <fcntl.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <time.h>
#include <syslog.h>
#include <dirent.h>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <fnmatch.h>
#include <ifaddrs.h>
#include <utime.h>
#include <pthread.h>

typedef unsigned char 	BYTE, *PBYTE;
typedef const char *	PCSTR;
typedef char *			PSTR;
typedef unsigned int 	UINT;
typedef unsigned char 	BOOL;
typedef unsigned short 		WORD, *PWORD;
typedef UINT 				DWORD, *PDWORD;

#define FALSE	0
#define TRUE	1



#define MAX_TRACE_SIZE			32
#define MAX_EXECUTE_ARGS_SIZE	0x100
#define TRUE 					1
#define FALSE					0
#define USECS_PER_SECOND		1000000
#endif