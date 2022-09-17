#ifndef __LIST_H
#define __LIST_H

// Check for stdlib
#ifndef EXIT_SUCCESS
#include <stdlib.h>
#endif

#include "Token.h"

// Growth factor of the array
#define R 2

// Dynamic Array to store Tokens
typedef struct List_t {
    size_t cap, len;
    Tok** data;
} List_t;

// Constructor
List_t* Cons(size_t c0) {
    List_t* xs = malloc(sizeof(List_t));
    if (!xs) {
        fprintf(stderr, "Could not allocate memory for the list of capacity: %lu \n", c0);
        exit(EXIT_FAILURE);
    }

    xs->data = malloc(sizeof(Tok*) * c0);

    xs->cap = c0;   
    xs->len = 0;

    return xs;
} 

// Append to the end of the list
void Append(List_t* xs, Tok* e) { 
    // Resize is needed
    if (xs->len == xs->cap) {
        xs->cap *= R;
        xs->data = realloc(xs->data, sizeof(Tok*) * xs->cap);
    }

    xs->data[xs->len++] = e;
}

// Get len of list
static inline size_t len(List_t* xs) { return xs->len; }

// Destructor
void Destroy(List_t* xs) {
    // Free tokens in the list
    for (size_t i = 0; i < len(xs); ++i)
        free(xs->data[i]);
    free(xs->data);
    free(xs);
}

#endif