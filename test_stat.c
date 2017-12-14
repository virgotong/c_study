#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int i;
	struct stat buf;
	char *ptr = NULL;

	if(argc != 2)
	{
		printf("usage: a.out <pathname>\n");
		exit(EXIT_FAILURE);
	}
	if(access(argv[1], R_OK) < 0)
	{
		printf("access error for: %s\n", argv[1]);
	}
	else
	{
		printf("Read access OK!\n");
	}
	/*for(i = 0; i < argc; i++)
	{
		printf("%s: ", argv[i]);
		if(lstat(argv[i], &buf) < 0)
		{
			printf("lstat err!");
		}		
		if(S_ISREG(buf.st_mode))
		{
			ptr = "regular";
		}
		else if(S_ISDIR(buf.st_mode))
		{
			ptr = "directory";
		}
		else if(S_ISCHR(buf.st_mode))
		{
			ptr = "character special";
		}
		else if(S_ISBLK(buf.st_mode))
		{
			ptr = "block special";
		}
		else if(S_ISFIFO(buf.st_mode))
		{
			ptr = "fifo";
		}
	#ifdef S_ISLNK
		else if(S_ISLNK(buf.st_mode))
		{
			ptr = "symbolic link";
		}
	#endif
	#ifdef S_ISSOCK
		else if(S_ISSOCK(buf.st_mode))
		{
			ptr = "socket";
		}
	#endif
		else
		{
			ptr = "unknown mode";
		}
		printf("%s\n", ptr);

		printf("blksize: %d\n", (int)buf.st_blksize);		
	}*/

	return 0;
}

