CFLAGS = -O3 -Wall -o atoc

all : atoc

run : ${PROG}
	./${PROG}

atoc : main.c atoc.c ops.c
	gcc ${CFLAGS} main.c atoc.c ops.c

clean :
	rm atoc
