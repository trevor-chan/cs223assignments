CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g3

all: Unit Blotto

Unit: smap_unit.o smap.o smap_test_functions.o
		${CC} ${CFLAGS} -o $@ $^ -lm

Blotto: blotto.o smap.o
		${CC} ${CFLAGS} -o $@ $^ -lm

smap_unit.o: smap_unit.c smap.h smap_test_functions.h

smap.o: smap.c smap.h
		${CC} ${CFLAGS} -c smap.c

smap_test_functions.o: smap_test_functions.c smap_test_functions.h
		${CC} ${CFLAGS} -c smap_test_functions.c

clean:
		$(RM) Unit Blotto *.o