#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef struct 
{
	void *key;
	size_t key_len;
} test;

void init( test *ptr, char *key, int key_len)
{
	ptr->key = key;
	ptr->key_len = key_len;	
}

void print(test *p)
{
	printf("Key: %s and key_len: %d\n", (char *)p->key, (int)p->key_len);
	printf("Key: %.*s\n", (int)p->key_len, (char *)p->key);
}


int main()
{
	char *key = "auscnash";
	int key_len = key ? (int)strlen(key) : 0;
	int i;
	for(i = 0; i < 1000; i++)
	{
		test A;
		memset(&A, 0, sizeof(A));
		init(&A, key, key_len);
		print(&A);
	}

	return 0;
}






