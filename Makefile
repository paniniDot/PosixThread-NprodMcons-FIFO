CFLAGS=-ansi -Wpedantic -Wall -Werror -D_THREAD_SAFE -D_REENTRANT -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread 
LFLAGS=

all: prodcons.exe

prodcons.exe: prodcons.o DBGpthread.o
	gcc ${LFLAGS} -o prodcons.exe prodcons.o DBGpthread.o ${LIBRARIES}

prodcons.o: prodcons.c DBGpthread.h
	gcc -c ${CFLAGS} prodcons.c 

DBGpthread.o: DBGpthread.c printerror.h
	gcc -c ${CFLAGS} DBGpthread.c

.PHONY: clean run

clean: 
	rm -f *.exe *.o *~ core

run: prodcons.exe
	./prodcons.exe   


