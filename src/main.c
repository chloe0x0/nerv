#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nerv.h"

/*
    A Brainfuck Interpreter using the Nerv API
*/

const char *USAGE = "usage: nerv <file> <[O0,O1,O2]>";

#define FB_SIZE 90000

Opt getop(char* arg)
{
    if (!strcmp(arg, "O0"))
    {
        return O0;
    }
    else if (!strcmp(arg, "O1"))
    {
        return O1;
    }
    else if (!strcmp(arg, "O2"))
    {
        return O2;
    }
    
    return O2;
}

int main(int argc, char *argv[])
{
    // no args provided
    if (argc == 1)
    {
        fprintf(stderr, "Not enough args provided\n");
        fprintf(stderr, USAGE);
        exit(EXIT_FAILURE);
    }

    char buffer[FB_SIZE];
    if (!Read_BF(argv[1], buffer, FB_SIZE))
    {
        fprintf(stderr, "Could not open %s!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // get optimization level
    Opt op = O2;

    if (argc >= 2)
    {
        op = getop(argv[2]);
    }  

    nerv(buffer, op);
}