.PHONY: all clean

CC := gcc
CFLAGS := -std=c99 -Wall -pedantic

all: main

main: main.o list.o
	$(CC) $(CFLAGS) -o $@ $^

list.o: list.c list.h

main.o: main.c list.h

list.o: list.c list.h

clean:
	rm *.o
	rm main