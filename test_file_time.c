#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void)
{
	char *p = "test_";
	time_t tm;
	tm = time(NULL);
	char buf[32];
	//char sbuf[32];
	// snprintf(buf, sizeof(buf), "%s%llu",p,(unsigned long long)tm);		
	snprintf(buf, sizeof(buf), "%s%llu",p,tm);		
	//snprintf(sbuf,sizeof(sbuf),"%s%lu",p,tm);
	printf ("%ld, %ld\n", sizeof(time_t), sizeof(size_t)) ;
	printf("buf: %s, %lu %lu\n",buf, strlen(buf), sizeof(buf));
}
