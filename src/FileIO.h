#ifndef _FIO_H
#define _FIO_H

#ifndef fopen
#include <stdlib.h>
#endif

/*
    Functions to handle File IO, namely: read and write to a buffer given a filepath/ filename
*/

/*
    Read a brainfuck file given a path and a buffer
    Returns a boolean indicating wether or not the file was succesfully read into the buffer
*/
bool Read_BF(const char* p, char* buff) {
    FILE* fp = fopen(p, "r");
    if (!fp) {
        return false;
    }

    char c;
    while( !feof(fp) ) {
        *buff++ = fgetc(fp);
    }
    *buff = '\0';

    return true;
}

#endif