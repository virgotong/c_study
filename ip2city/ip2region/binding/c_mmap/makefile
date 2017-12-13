ip2city: testSearcher.o ip2region.o
	gcc -Wall testSearcher.o ip2region.o -o ip2city

testSearcher.o: testSearcher.c ip2region.h
	gcc -Wall -c testSearcher.c 

ip2region.o: ip2region.c ip2region.h
	gcc -Wall -c ip2region.c
