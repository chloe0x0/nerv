#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "List.h"
#include "FileIO.h"

// Constants
#define TAPE_LEN 30000

typedef enum TOKEN_TYPE {
    SUM,            // Add x to the current cell
    SUB,            // Subtract x from the current cell
    LOOP_START,     // Mark the begining of a loop
    LOOP_END,       // Mark the end of a loop
    SHR,            // Shift the memory ptr to the right by x
    SHL,            // Shift the memory ptr to the left by x
    OUT,            // Print cell value as ASCII 
    IN,             // User I/O
    COM,            // Comment
    NIL,            // Set the current memory cell value to 0
} TOKEN_TYPE;

// Lookup table to print the token type as a string
const char* Flag_LT[9] = {"SUM", "SUB", "LOOP_START", "LOOP_END", "SHR", "SHL", "OUT", "IN", "COM"};

typedef struct Token_t {
    TOKEN_TYPE flag;
    int n;              // number of times to apply the operation (computed by constant folding)
    Node_t* jump;       // the position to jump if the current token is a loop
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

// Function to compute loop jumps
// An alternative method uses Lookup Tables
// The problem is that this often uses more space than necessary
void Comp_Loops(List_t* Tokens) {
    Node_t* tmp = Tokens->root;
    for (size_t i = 0; i < len(Tokens); ++i) {
        if (((Token_t*)tmp->data)->flag != LOOP_START) { continue; }
        
        // Scan ahead for next matching loop end token
        int count = 1;
        int scan = 1;
        Node_t* search = tmp->nxt;
        while (count) {
            TOKEN_TYPE tmp = ((Token_t*)search->data)->flag;
            count += (tmp == LOOP_START) + (-1 * (tmp==LOOP_END));
            scan++;
            search = search->nxt;
        }
        ((Token_t*)tmp->data)->jump = search;
        ((Token_t*)search->data)->jump = tmp;

        tmp = tmp->nxt;
    }
}

// Lexer
/*
    Next major optimization: Loop Abstractions

    Common loop structures can be mapped to single operations

    [-] -> sets the current cell to 0
                NIL
    [->+<] -> moves the current cell value to the right by one cell
                MMOV_RIGHT 1 
    [->*n + <*n] -> moves the current cell value to the right by N cells
        NMMOV_RIGHT N

*/
List_t* Lexer(const char* p) {
    List_t* Tokens = Cons();

    size_t ip = 0;
    while (ip < strlen(p)) {
        Token_t* t = malloc(sizeof(Token_t));
        t->jump = 0;
        t->n = 1;

        switch (p[ip]) {
            case '+':
                t->flag = SUM;
                while(p[++ip] == '+') {
                    t->n++;
                }
                ip--;
                break;
            case '-':
                t->flag = SUB;
                while (p[++ip] == '-') {
                    t->n++;
                }
                ip--;
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
                while (p[++ip] == '>') {
                    t->n++;
                }
                ip--;
                break;
            case '<':
                t->flag = SHL;
                while (p[++ip] == '<') {
                    t->n++;
                }
                ip--;
                break;
            default:
                t->flag = COM;
                break;
            
        }
        ip++;

        if (t->flag == COM) { free(t); continue; } // Ignore other characters as comments
        Append(Tokens, (void*)t);
    } 
    
    // Compute loop jumps
    Comp_Loops(Tokens);

    return Tokens;
}

// Basic Interpreter
void Interp(const char* p) {
    List_t* tokens = Lexer(p);

    char mem[TAPE_LEN] = {0};
    Node_t* instr = tokens->root;
    size_t mem_ptr = 0;

    Token_t* tmp;
    while ( instr ) {
        tmp = ((Token_t*)instr->data)->flag;
        switch ( tmp->flag ) {
            case SUM:
                mem[mem_ptr] += tmp->n;
                break;
            case SUB:
                mem[mem_ptr] -= tmp->n;
                break;
            case SHR:
                mem_ptr += tmp->n;
                break;
            case SHL:
                mem_ptr -= tmp->n;
                break;
            case LOOP_START:
                if (!mem[mem_ptr]) {
                    instr = tmp->jump;
                }
                break;
            case LOOP_END:
                if (mem[mem_ptr]) {
                    instr = tmp->jump;
                }
                break;
            case IN:
                mem[mem_ptr] = getchar();
                break;
            case OUT:
                putchar(mem[mem_ptr]);
                break;
            case NIL:
                mem[mem_ptr] = 0;
                break;                
            default:
                break;
        }
    
        instr = instr->nxt;
    }

    Destroy(tokens);
}

// Visualize an expression
void Visualize_Expr(List_t* expr_tokens) {
    Node_t* tmp = expr_tokens->root;
    printf("expr :== \n \t | \n");
    for (size_t i = 0; i < len(expr_tokens); ++i) {
        Token_t* t = (Token_t*)tmp->data;
        printf("\t %s %d", Flag_LT[t->flag], t->n);
        if (t->flag == LOOP_START || t->flag == LOOP_END)
            printf(" jumps to: %d", t->jump);
        printf("\n");
    }
}

// Helper function to ensure that incoming expressions have well formed loops
bool validate_loops(const char* prog) {
    size_t l, r;
    l = r = 0;

    const char* tmp = prog;

    do {
        l += (*tmp == ']');
        r += (*tmp == '[');
    }while(*(tmp++) != '\0');

    return l == r;
}

// Can interpret the tokens, or compile to C code to further optimize

int main(int argc, char* argv[]) {
    char* str = malloc(sizeof(char) * 999999);
    if (!Read_BF(argv[1], str)) {
        fprintf(stderr, "Could not read file! \n");
        exit(EXIT_FAILURE);
    }

    const char* prog = str;
    if (!validate_loops(prog)) {
        fprintf(stderr, "Invalid parens! \n");
        exit(EXIT_FAILURE);
    }
    
    Interp(prog);
    free(str);
}
