#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct 
{
	char name;
	int age;
	//char *address;
	char address[20];
}test, *ptest;

int print_person(test ptr)
{
	printf("name: %c\n", ptr.name);
	printf("age: %d\n", ptr.age);
	printf("address: %s\n", ptr.address);
	return 0;
}

int test_struct( int argc, char *argv[])
{
	test default_test = 
	{	
		.name = 'a',
		.age = 10,
		.address = "172.18.10.1",
	};

	print_person( default_test );

	return 0;	
}



