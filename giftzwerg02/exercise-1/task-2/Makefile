CC = gcc

all: generator supervisor 

%.o: %.c
	$(CC) -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIC_C_SOURCE=200809L -g -c $^

supervisor: supervisor.o
	$(CC) -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIC_C_SOURCE=200809L -g -o $@ $^

generator: generator.o
	$(CC) -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIC_C_SOURCE=200809L -g -o $@ $^

clean:
	rm -rf *.o generator supervisor
