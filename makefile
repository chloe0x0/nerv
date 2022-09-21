# Barebones makefile, make it better later !!

CC = gcc
CFLAGS = -O2 -o nerv
REMOVE = del # rm in Linux

all:
	$(CC) $(CFLAGS) ./src/nerv.c ./src/List.c

clean:
	$(REMOVE) *.exe
