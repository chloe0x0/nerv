# Barebones makefile, make it better later !!

CC = gcc
CFLAGS = -Wall -Wextra -O2
REMOVE = del # rm -f in Linux
FILES = ./src/nerv.c ./src/List.c

all:
	$(CC) $(CFLAGS) -o nerv ./src/main.c $(FILES) 

test:
	$(CC) $(CFLAGS) -o test ./src/test.c $(FILES) 

debug:
	$(CC) $(CFLAGS) -o db ./src/debug.c $(FILES)

clean:
	$(REMOVE) *.exe
	$(REMOVE) tmp.out
	$(REMOVE) *.o