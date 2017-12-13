#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

void save_file(char *name, unsigned char *content, int len)
{
	FILE *fp;
	if(len < 0)
	{
		if((fp = fopen(name,"w")))
		{
			fprintf(fp, "%s", content);
			fclose(fp);			
		}
	}
	else
	{
		if((fp = fopen(name, "wb")))
		{
			fwrite(content, len, 1, fp);
			fclose(fp);
		}
	}
}

int main(void)
{
	/*char *pos, *next;
	char *buff = (char *)malloc(32);
	strcpy(buff, "this is \n a test");

	FILE *fp;
	fp = fopen("/tmp/test_string.txt","a");

	for(pos = buff; pos && *pos; pos = next)
	{
		next = strchr(pos, '\n');
		if(next) *next++ = 0;
		fprintf(fp, "%s\n", pos);			
		//fprintf(stdout, "%s\n", pos);			
	}
	if(buff) free(buff);
	buff = NULL;
	fclose(fp);*/
	char *name = "/tmp/test_save_file.txt";
	unsigned char *content = (unsigned char*)"test save file func";
	save_file(name, content, -1);
	return 0;
}
