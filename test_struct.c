#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct 
{
	char name;
	int age;
}test, *ptest;

int print_person(test *ptr)
{
	return 0;
}

int main()
{
	test person[10];
	int i;	
	for(i = 0; i < 10; i++)
	{
		person[i].name = 's';
		person[i].age = i+1;
	}

	int size = (int)(sizeof(person) / sizeof(person[0]));
	printf("size: %d\n", size);

	print_person(person);
	return 0;	
}



