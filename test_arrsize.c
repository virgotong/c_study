#include "global.h"

void test_arrsize( void )
{
	//char arr[100] = "asxasxas";
	//printf("%d %d\n", sizeof(arr), (sizeof(arr)/sizeof(arr[0])));

	int a[10] = {1,2,3,4,5,6,7,8,9,0};
	int *p = a;
	
	printf("p: %p\n",p);
	p += 5;
	printf("p+5: %p\n",p);
}
