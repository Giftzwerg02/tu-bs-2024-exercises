.PHONY: all clean

CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g

all: mygrep

mygrep: main.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c

clean:
	rm -rf mygrep *.o