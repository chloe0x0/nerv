#include <stdio.h>
#include "List.h"

typedef enum TOKEN_TYPE {
    SUM,            // Add x to the current cell
    SUB,            // Subtract x from the current cell
    LOOP_START,     // Mark the begining of a loop
    LOOP_END,       // Mark the end of a loop
    SHR,            // Shift the memory ptr to the right by x
    SHL,            // Shift the memory ptr to the left by x
    OUT,            // Print cell value as ASCII 
    IN,             // User I/O
    COM             // Comment
} TOKEN_TYPE;

typedef struct Token_t {
    TOKEN_TYPE flag;
    int n;                      // number of times to apply the operation (computed by constant folding)
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

    const char* ip = p;
    while (*ip != '\0') {
        Token_t* t = malloc(sizeof(Token_t));
        t->n = t->jump = 0;

        switch (*ip) {
            case '+':
                t->flag = SUM;
                while(*(++ip) == '+') {
                    t->n++;
                }
                break;
            case '-':
                t->flag = SUB;
                while (*(++ip) == '-') {
                    t->n++;
                }
                break;
            case '.':
                t->flag = OUT;
                break;
            case ',':
                t->flag = IN;
                break;
            case '[':
                t->flag = LOOP_START;
                break;
            case ']':
                t->flag = LOOP_END;
                break;
            case '>':
                t->flag = SHR;
                while (*(++ip) == '>') {
                    t->n++;
                }
                break;
            case '<':
                t->flag = SHL;
                while (*(++ip) == '<') {
                    t->n++;
                }
                break;
            default:
                t->flag = COM;
                break;
        }
        
        if (t->flag == COM) { free(t); continue; } // Ignore other characters as comments
        Append(Tokens, (void*)t);
    } 
 
    // Now to precompute loop jumps
    // An alternative method use Lookup Tables
    // The problem is that this often uses more space than necessary
    for (size_t i = 0; i < len(Tokens); ++i) {
        if (((Token_t*)Tokens->data[i])->flag != LOOP_START) { continue; }
        
        // Scan ahead for next matching loop end token
        int count = 1;
        int scan = 1;
        while (count) {
            TOKEN_TYPE tmp = ((Token_t*)Tokens->data[i + scan])->flag;
            count += (tmp == LOOP_START) + (-1*tmp==LOOP_END);
            scan++;
        }
        ((Token_t*)Tokens->data[i])->jump = scan + i;
    }

    return Tokens;
}


// Can interpret the tokens, or compile to C code to further optimize

int main(void) {
    const char* prog = "++++-";
    List_t* tokens = Lexer(prog);
    printf("Lexer produced: %d tokens \n", len(tokens));
    printf("Hello Nerv!!");
}
