main:	main.o test_struct.o 
	gcc -Wall main.o test_struct.o -o main -g

main.o:	main.c global.h 
	gcc -Wall -c main.c -g

test_struct.o: test_struct.c global.h
	gcc -Wall -c test_struct.c -g
