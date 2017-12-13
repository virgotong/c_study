main:	main.o test_funca.o test_funcb.o test_arrsize.o test_list.o
	gcc -Wall main.o test_funca.o test_funcb.o test_arrsize.o test_list.o -o main -g

main.o:	main.c global.h 
	gcc -Wall -c main.c -g

test_funca.o:	test_funca.c global.h
	gcc -Wall -c test_funca.c

test_funcb.o:	test_funcb.c global.h
	gcc -Wall -c test_funcb.c 

test_arrsize.o:	test_arrsize.c global.h
	gcc -Wall -c test_arrsize.c

test_list:	test_list.c global.h
	gcc -Wall -c test_list.c -g

