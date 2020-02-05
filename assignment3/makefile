CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g3

all: Unit Heatmap

Unit: track_unit.o track.o trackpoint.o location.o
		${CC} ${CFLAGS} -o $@ $^ -lm

Heatmap: heatmap.o track.o trackpoint.o location.o
		${CC} ${CFLAGS} -o $@ $^ -lm

heatmap.o: heatmap.c track.h trackpoint.h location.h

track_unit.o: track_unit.c track.h trackpoint.h location.h

track.o: track.c track.h
		${CC} ${CFLAGS} -c track.c

trackpoint.o: trackpoint.c trackpoint.h
		${CC} ${CFLAGS} -c trackpoint.c

location.o: location.c location.h
		${CC} ${CFLAGS} -c location.c

clean:
		$(RM) track *.o