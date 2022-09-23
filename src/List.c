#include <stdlib.h>
#include <stdio.h>
#include "Token.h"
#include "List.h"

// Constructor
List_t* Cons(size_t c0) {
    List_t* xs = malloc(sizeof(List_t));
    if (!xs) {
        fprintf(stderr, "Could not allocate memory for the list\n");
        exit(EXIT_FAILURE);
    }

    xs->data = malloc(sizeof(Tok*) * c0);
    if (!xs->data) {
        fprintf(stderr, "Could not allocate memory for the list of capacity: %zu \n", c0);
        exit(EXIT_FAILURE);
    }

    xs->cap = c0;   
    xs->len = 0;

    return xs;
} 

// Append to the end of the list
void Append(List_t* xs, Tok* e) { 
    // Resize is needed
    if (xs->len == xs->cap) {
        xs->cap *= R;
        xs->data = realloc(xs->data, sizeof(Tok*) * xs->cap+1);

        if (!xs->data) {
            fprintf(stderr, "Could not reallocate memory for the list of capacity: %zu\n", xs->cap);
            exit(EXIT_FAILURE);
        }
    }

    xs->data[xs->len++] = e;
}

// Destructor
void Destroy(List_t* xs) {
    // Free tokens in the list
    for (size_t i = 0; i < len(xs); ++i)
        free(xs->data[i]);
    free(xs->data);
    free(xs);
}
