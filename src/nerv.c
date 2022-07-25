#include <stdio.h>
#include "List.h"

typedef enum TOKEN_TYPE {
    SUM,            // Add x to the current cell
    SUB,            // Subtract x from the current cell
    LOOP_START,     // Mark the begining of a loop
    LOOP_END,       // Mark the end of a loop
    SHR,            // Shift the memory ptr to the right by x
    SHL             // Shift the memory ptr to the left by x
} TOKEN_TYPE;

typedef struct Token_t {
    TOKEN_TYPE flag;
    int n;          // number of times to apply the operation (computed by constant folding)
    int jump;       // the position to jump if the current token is a loop
} Token_t;

// Convert Brainfuck Code to a set of Tokens
// Use of constant folding to reduce redundancy and optimize the program
// A basic example: 

/*
 *  +++++ would convert to a single Token of the following form
 *
 *  Token
 *      {
 *          flag = SUM,
 *          lexeme = +++++,
 *          n = 5
 *      }
 */
// In this way, rather than applying SUM to the current cell N times we simply add N to the cell once 
// The asymptotic time complexity of c + n, where c is the current cell, goes from linear to constant
// The secondary optimization to start with will be storing loop jump locations in the token structures
// This eliminates the need to repeatedley scan back and forth to find matching tokens, an expensive and unnecessary computation

// Lexer
List_t* Lexer(const char* p) {
    List_t* Tokens = Cons(25);

    return Tokens;
}


// Can interpret the tokens, or compile to C code to further optimize

int main(void) {
    printf("Hello Nerv!!");
}
