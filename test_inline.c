#include <stdio.h>
#include <string.h>
#include <float.h>
static inline char *f_to_a( float data)
{
	static char buf[32];
	//static char *buf = (char *)malloc() 	
	snprintf(buf, sizeof(buf), "%.2f", data);
	printf("%s\n", buf);
	return buf;
}

static inline char *lu_to_a( unsigned long long data)
{
	static char buf[32];
	snprintf(buf, sizeof(buf), "%llu", data);
	return buf;
}

int main(int argc, char *argv[])
{
	float a = 2123512343.12313;
	unsigned long b = 655442564;
	//double c = (double)a;
	printf("%lf\n",(double)a);
	printf("%lld\n",FLT_MAX);
	// f_to_a(a);
	//printf("f_to_a: %s\n", f_to_a(a));
	//printf("lu_to_a: %s\n", lu_to_a(b));
}
