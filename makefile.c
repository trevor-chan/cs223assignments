CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g3

SplitAudio: split.o output.o
	${CC} ${CFLAGS} -o $@ $^

split.o: output.h
output.o: output.h
