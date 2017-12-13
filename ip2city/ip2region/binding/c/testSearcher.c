#include "ip2region.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

//read a line from a command line.
static char *getLine( FILE *fp, char *__dst ) 
{
    register int c;
    register char *cs;

    cs = __dst;
    while ( ( c = getc( fp ) ) != EOF ) {
        if ( c == '\n' ) break;
        *cs++ = c; 
    }
    *cs = '\0';

    return ( c == EOF && cs == __dst ) ? NULL : __dst;
}

static double getTime()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);

    return (tv.tv_sec * 1000 + ((double)tv.tv_usec)/1000);
}

int main( int argc, char **argv )
{
    ip2region_entry ip2rEntry;
    datablock_entry datablock;
    char *dbFile = NULL, *algorithm = NULL;
    char line[256];
    uint_t (*func_ptr)(ip2region_t, char *, datablock_t);
    double s_time, c_time;
    memset(&datablock, 0x00, sizeof(datablock_entry));
    
    dbFile = "/home/debug/ip2city/ip2region/data/ip2region.db";

    algorithm = "B-tree";
    func_ptr  = ip2region_btree_search_string;

    if(argc > 1)
    {
        if(strcasecmp(argv[1], "binary") == 0)
        {
            algorithm = "Binary";
            func_ptr = ip2region_binary_search_string;
        }
        else if( strcasecmp(argv[1], "memory") == 0)
        {
            algorithm = "Memory";
            func_ptr = ip2region_memory_search_string;
        }        
    }

    //create a new ip2rObj
    printf("+--initializing  %s ... \n", algorithm);
	println("Please enter quit to exit...");
    if ( ip2region_create(&ip2rEntry, dbFile) == 0 ) {
        println("Error: Fail to create the ip2region object\n");
        return 0;
    }

    while ( 1 ) {
        print("ip2region>> ");
        getLine( stdin, line );
		printf("length: %d\n", strlen(line));
        if ( strlen(line) < 2 ) continue;
        if ( strcasecmp( line, "quit" ) == 0 ) {
            println("+--Bye!");
            break;
        }

        s_time = getTime();
        func_ptr(&ip2rEntry, line, &datablock);
        c_time = getTime() - s_time;
        printf("%d|%s in %.5f millseconds\n", datablock.city_id, datablock.region, c_time);
    }

    //destory the ip2rObj
    ip2region_destroy(&ip2rEntry);

    return 0;
}
