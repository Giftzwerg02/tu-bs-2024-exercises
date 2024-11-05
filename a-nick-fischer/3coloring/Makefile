.PHONY: all clean

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g

all: 3coloring

3coloring: main.o graph.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c

graph.o: graph.c graph.h

clean:
	rm -rf mygrep *.o
	rm 3coloring