#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define RD_KAFKA_PARTITION_UA  ((int32_t)-1)

static inline char *rand_num( int a, int b)
{	
	static char num[10];	
	int i = rand()%(b -a) + a;
	snprintf(num, sizeof(num), "%d", i);
	return num;
}

int main(int argc, char *argv[])
{
	
	srand((unsigned)time(NULL));
	char key[20];
	strcpy(key, "test@");	
	strcat(key, rand_num(100,1000));
	printf("%s\n", key);
	int key_len = (int)strlen(key);
	printf("%d\n", key_len);		
	return 0;
}