.PHONY: all

CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

all: main

main: main.o list.o
	$(CC) $(CFLAGS) -o $@ $^

# list.o: list.c list.h
# 	gcc -std=c99 -Wall -pedantic -c -o $@ $<
#
# main.o: main.c list.h
# 	gcc -std=c99 -Wall -pedantic -c -o $@ $<
#
# list.o: list.c list.h

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm main *.o
