#ifndef __LIST_H
#define __LIST_H

// Check for stdlib
#ifndef EXIT_SUCCESS
#include <stdlib.h>
#endif

// Growth factor of the array
#define R 2

// Type generic List structure implemented with a Dynamic Array Data Structure
typedef struct List_t {
    size_t cap, len;
    void** data;
} List_t;

// Constructor
List_t* Cons(size_t c0) {
    List_t* xs = malloc(sizeof(List_t));

    xs->data = calloc(c0, sizeof(void*));

    xs->cap = c0;   
    xs->len = 0;

    return xs;
} 

// Append to the end of the list
void Append(List_t* xs, void* e) { 
    // Resize is needed
    if (xs->len == xs->cap) {
        xs->cap *= R;
        void** new_array = malloc(sizeof(void*) * xs->cap);

        memcpy(new_array, xs->data, sizeof(void*) * xs->len);
        free(xs->data);

        xs->data = new_array;
    }

    xs->data[xs->len++] = e;
}

// Get len of list
static inline size_t len(List_t* xs) { return xs->len; }

// Destructor
void Destroy(List_t* xs) {
    // Free tokens in the list
    for (size_t i = 0; i < len(tokens); ++i)
        free((Token_t*)tokens->data[i]);
    free(xs->data);
    free(xs);
}

#endif