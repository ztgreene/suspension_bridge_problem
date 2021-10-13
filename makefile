COMPILER = gcc
CFLAGS = -Wall -pedantic -pthread

EXES = bridge

all: ${EXES}

bridge: bridge.c
	${COMPILER} ${CFLAGS}  bridge.c -o bridge
clean: 
	rm -f *~ *.o ${EXES}
run:
	./bridge
