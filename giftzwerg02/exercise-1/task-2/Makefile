.PHONY: clean

CC = gcc
CCFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIC_C_SOURCE=200809L -g

all: generator supervisor 

%.o: %.c
	$(CC) $(CCFLAGS) -c $^

supervisor: supervisor.o

generator: generator.o

clean:
	rm -rf *.o generator supervisor
