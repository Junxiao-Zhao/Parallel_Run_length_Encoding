CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyuenc

nyuenc: nyuenc.c global.h parallel.c parallel.h sequential.c sequential.h thread.c thread.h
	gcc -pthread -o nyuenc nyuenc.c parallel.c sequential.c thread.c

.PHONY: clean
clean:
	rm -f *.o nyuenc
