# Barebones makefile, make it better later !!

CC = gcc
CFLAGS = -Wall -O2 -o nerv
REMOVE = del # rm in Linux

all:
	$(CC) $(CFLAGS) ./src/main.c ./src/nerv.c ./src/List.c

clean:
	$(REMOVE) *.exe
	$(REMOVE) out.c