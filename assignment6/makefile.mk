CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g3

all: TSP Unit

TSP: TSP.o lugraph.o location.o
	${CC} ${CFLAGS} -o $@ $^ -lm

Unit: lugraph_unit.o lugraph.o location.o
	${CC} ${CFLAGS} -o $@ $^ -lm

TSP.o: TSP.c

lugraph_unit.o: lugraph_unit.c lugraph.h

lugraph.o: lugraph.c lugraph.h

location.o: location.c location.h
	${CC} ${CFLAGS} -c location.c -lm

clean:
	$(RM) TSP Unit *.o