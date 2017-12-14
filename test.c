#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static inline char *rand_num(int a, int b)
{
		static char buf[5];
			int num = rand()%(b -a) + a;
				snprintf(buf, sizeof(buf), "%d", num);
					return buf;
}

static inline char *lu_to_a( unsigned long long data )
{
	static char buf[32];
	snprintf(buf, sizeof(buf), "%llu", data);
	return buf;
}

#define check_rval( call, type ) \
		do { \
			        if ((call) != 0) { \
						            printf("Set field value %s has an error\n", type); \
						            exit(EXIT_FAILURE); \
						        } \
			    } while (0);




int main(int argc, char *argv[])
{
	/*int i;	
	//srand((unsigned)time(NULL));
	for(i = 0; i < 20; i++)
	{
		if(i % 2 != 0)
		{
		//	continue;
		}
		printf("i: %d\n",i);
	}
	

	int rval = 1;
	//check_rval(rval,"test");*/
	printf("pid: %d  uid: %d, gid: %d\n", getpid(), getuid(), getgid());
	return 0;
}















