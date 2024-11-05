.PHONY: clean

CC = gcc
CCFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIC_C_SOURCE=200809L -g
DEPS = lib.h
OBJS = lib.o

all: generator supervisor 

debug: CCFLAGS += -DDEBUG
debug: all

%.o: %.c $(DEPS)
	$(CC) $(CCFLAGS) -c $^ 

supervisor: supervisor.o $(OBJS)

generator: generator.o $(OBJS)

clean:
	rm -rf *.o generator supervisor
