#ifndef __FARTHEST_H__
#define __FARTHEST_H__
#include "distance.h"

int farthest(int index, const city *carray, int clistlength);

#endif

/*

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g3

TSP: TSP.o nearest.o farthest.o distance.o simplenear.o insertnear.o
		${CC} ${CFLAGS} -o $@ $^ -lm
TSP.o: TSP.c 

nearest.o: nearest.c nearest.h distance.h

farthest.o: farthest.c farthest.h distance.h

distance.o: distance.c distance.h

simplenear.o: simplenear.c simplenear.h

insertnear.o: insertnear.c insertnear.h

clean:
		$(RM) TSP *.o

*/