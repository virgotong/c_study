#include "global.h"

int main( int argc, char *argv[])
{
	/*printf("%s(%d) %s\n",__FILE__, __LINE__, __FUNCTION__);
	test_funca();

	printf("%s(%d) %s\n", __FILE__, __LINE__, __FUNCTION__);
	test_funcb();
	test_arrsize();	*/

	test_list(argc, argv);

	return 0;
}
