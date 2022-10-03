#ifndef __LIST_H
#define __LIST_H

#include "Token.h"

// Growth factor of the array
#define R 2

// Dynamic Array to store Tokens
typedef struct List_t
{
    size_t cap, len;
    Tok **data;
} List_t;

// Constructor
List_t *Cons(size_t);
// Destructor
void Destroy(List_t *);
// Append to the end of the list
void Append(List_t *, Tok *);
// Get len of list
inline size_t len(List_t *xs) { return xs->len; }

#endif