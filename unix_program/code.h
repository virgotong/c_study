#ifndef _CODE_H_
#define _CODE_H_

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXLINE	4096

/*
S_IRUSR
  Permits the file's owner to read it.

S_IWUSR
  Permits the file's owner to write to it.

S_IRGRP
  Permits the file's group to read it.

S_IWGRP
  Permits the file's group to write to it.
*/
#define FILE_MODE	( S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH )
#define DIR_MODE	( FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH )

typedef void 	Sigfunc( int );

#if defined( SIG_IGN ) && !defined( SIG_ERR )
#define SIG_ERR		((Sigfunc *)-1)
#endif

#define MIN(a,b)	( ( a ) < ( b ) ? ( a ) : ( b ) )
#define MAX(a,b)	( ( a ) > ( b ) ? ( a ) : ( b ) )




#endif