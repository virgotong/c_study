#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void test( int sig)
{
	printf("get a signal:%d\n", sig);
}


int main(int argc, char *argv[])
{
	signal(SIGINT, test);
	while(1)
	{
		printf("***\n");
		sleep(1);
	}

	return 0;
}
