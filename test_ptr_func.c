#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef void (*p_int)(void);
typedef void (*p_string)(char *name);

typedef struct
{
	char *name;
	void (*_p_int)(void);
	void (*_p_string)(char *name);
} Print1;

typedef struct 
{	
	char *name;	
	p_int 		_int_;
	p_string 	_string_;
} Print2;

//static const Print *driver = NULL;

void print_int( void )
{
	int i;
	int a[10] = {1,2,3,4,5,6,7,8,9,10};
	for(i = 0; i < 10; i++)
	{
		printf("print int data: %d\n", a[i]);
	}
}

void print_string( char *name )
{
	char *p = (char *)malloc(100);
	memcpy(p, name, strlen(name));
	printf("print string: %s\n", p);

	if(p)
	{
		free(p);
	}

	p = NULL;
}

int main(int argc, char *argv[])
{

	const Print1 R_print_func_1 = {
		"type1",
		print_int,
		print_string
	};

	const Print2 R_print_func_2 = {
		"type2",
		print_int,
		print_string,
	};

	/* 通过函数指针访问函数方式一 */
	static const Print1 *driver = NULL;
	driver = &R_print_func_1;

	printf("******方式一******\n");
	driver->_p_int();

	char *str = driver->name;	
	driver->_p_string( str );

	printf("\n");

	/* 方式二 */
	p_int process_int = NULL;
	process_int = R_print_func_2._int_;
		
	p_string process_str = NULL;
	process_str = R_print_func_2._string_;

	if(process_int || process_str)
	{
		printf("******方式二******\n");
		process_int();
		
		process_str(R_print_func_2.name);		
	}

	return 0;
}