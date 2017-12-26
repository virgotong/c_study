#include "global.h"

int main( int argc, char *argv[])
{
	/*printf("%s(%d) %s\n",__FILE__, __LINE__, __FUNCTION__);
	test_funca();

	printf("%s(%d) %s\n", __FILE__, __LINE__, __FUNCTION__);
	test_funcb();
	test_arrsize();	*/

	printf("%d\n", argc);
	printf("%c\n", argv[1][0]);
	//test_list(argc, argv);


	return 0;
}
