#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int get_param( const char *brokerlist)
{
	char *s_copy = strdup(brokerlist);
	char *s = s_copy;

	printf("brokerlist: %s\n", s);
	
	// while ( *s )
	// {
	// 	if( *s == ',' || *s == ' ' )
	// 	{
	// 		s++;
	// 		continue;
	// 	}
	// 	printf("%s\n", s);
	// }

	free(s_copy);

	return 0;
}


int main(int argc , char *argv[])
{
	int opt;
	char *brokers = NULL;
	char mode = 'P';
	while( (opt = getopt(argc, argv, "PCb:t:p:")) != -1 )
	{
		switch( opt )
		{
			case 'P': 
			case 'C':
				mode  = opt;
				break;
			case 'b':
				//printf("brokers: %s\n", optarg);
				brokers = optarg;
				get_param( brokers );
				break;
			case 't':
				printf("topic: %s\n", optarg);
				break;
			case 'p':
				printf("partitions: %d\n", atoi(optarg));

			default:
				printf("Please Enter a Param\n");
				break;
		}
	}

	if( mode  == 'P' )
	{
		get_param( brokers );
	}
	return 0;
}
