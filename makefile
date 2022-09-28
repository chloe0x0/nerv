# Barebones makefile, make it better later !!

CC = gcc
CFLAGS = -Wall -O2
REMOVE = del # rm -f in Linux

all:
	$(CC) $(CFLAGS) -o nerv ./src/main.c ./src/nerv.c ./src/List.c

test:
	$(CC) $(CFLAGS) -o test ./src/test.c ./src/nerv.c ./src/List.c

clean:
	$(REMOVE) *.exe