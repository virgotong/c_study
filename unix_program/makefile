#OBJECTS := code hash msgsend msgrecive unix
OBJECTS := unix

.PHONY : all

all:
	@for TARGET in $(OBJECTS);						\
	do												\
		make -f makefile-pro object_type=$$TARGET;	\
		if [ $$? -ne 0 ];							\
		then										\
			exit 1;									\
		fi;											\
	done

clean :
	rm -f *.o
	rm -f *.d
	rm -f code hash msgrecive msgsend unix
