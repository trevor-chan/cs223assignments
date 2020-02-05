CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g3

Unit: lugraph_unit.o lugraph.o location.o
	${CC} ${CFLAGS} -o $@ $^

lugraph_unit.o: lugraph_unit.c lugraph.h
	${CC} ${CFLAGS} -c $@ $^

lugraph.o: lugraph.c lugraph.h
	${CC} ${CFLAGS} -c $@ $^

location.o: location.c location.h
	${CC} ${CFLAGS} -c $@ $^

clean:
	$(RM) TSP Unit *.o