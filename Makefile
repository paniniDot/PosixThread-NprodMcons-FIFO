CFLAGS=-ansi -Wpedantic -Wall -Werror -D_THREAD_SAFE -D_REENTRANT -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread 
LFLAGS=

all: prodcons.exe

prodcons.exe: prodcons.o DBGpthread.o
	gcc ${LFLAGS} -o prodcons.exe ./bin/prodcons.o ./bin/DBGpthread.o ${LIBRARIES}
	mv prodcons.exe ./bin

prodcons.o: ./src/prodcons.c ./src/DBGpthread.h
	gcc -c ${CFLAGS} ./src/prodcons.c 
	mv prodcons.o ./bin

DBGpthread.o: ./src/DBGpthread.c ./src/printerror.h
	gcc -c ${CFLAGS} ./src/DBGpthread.c
	mv DBGpthread.o ./bin

.PHONY: clean run

clean: 
	rm -f ./bin/*.exe ./bin/*.o ./bin/*~ ./bin/core

run: prodcons.exe
	./bin/prodcons.exe   


