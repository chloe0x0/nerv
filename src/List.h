#ifndef __LIST_H
#define __LIST_H

// Check for stdlib
#ifndef EXIT_SUCCESS
#include <stdlib.h>
#endif

// Type generic List structure implemented with a Singly Linked List
typedef struct Node_t {
    void* data;
    struct Node_t* nxt;
} Node_t;

typedef struct List_t {
    size_t len;
    Node_t* root, *tail;
} List_t;

// Constructor
List_t* Cons() {
    List_t* xs = malloc(sizeof(List_t));

    xs->root = xs->tail = NULL;
    xs->len = 0;

    return xs;
} 

// Append to the end of the list
void Append(List_t* xs, void* e) { 
    Node_t* n = malloc(sizeof(Node_t));
    n->data = e;
    n->nxt = NULL;

    if (!xs->root) {
        xs->root = xs->tail = n;
    }else {
        xs->tail->nxt = n;
        xs->tail = n;
    }

    ++xs->len;
}

// Get len of list
static inline size_t len(List_t* xs) { return xs->len; }

// Destructor
void Destroy(List_t* xs) {
    Node_t* curr = xs->root;
    Node_t* tmp = curr->nxt;
    for (size_t i = 0; i < xs->len; ++i) {
        free(curr->data);
        free(curr);
        curr = tmp;
        tmp = tmp->nxt;
    }
    free(xs);
}

#endif
