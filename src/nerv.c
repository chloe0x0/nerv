#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "FileIO.h"
#include "List.h"
#include "Token.h"

// Constants
#define TAPE_LEN 30000
#define DEF_OUT  "out.c"

// Lookup table to print the token type as a string
const char* Flag_LT[10] = {"SUM", "SUB", "LOOP_START", "LOOP_END", "SHR", "SHL", "OUT", "IN", "COM", "MEM_SET"};

// Convert Brainfuck Code to a set of Tokens
// Use of run length encoding to reduce redundancy and optimize the program
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
// The secondary optimization to start with will be storing loop offset locations in the token structures
// This eliminates the need to repeatedley scan back and forth to find matching tokens, an expensive and unnecessary computation

// Function to compute loop offsets
// An alternative method uses Lookup Tables
// The problem is that this often uses more space than necessary
void Comp_Loops(List_t* Tokens) {
    for (size_t i = 0; i < len(Tokens); ++i) {
        Tok* token = Tokens->data[i];
        if (token->flag != LOOP_START) { continue; }
        
        // Scan ahead for next matching loop end token
        int count = 1;
        int scan = 1;
        while (count) {
            TOKEN_TYPE tmp = (Tokens->data[i + scan])->flag;
            count += (tmp == LOOP_START) + (-1 * (tmp==LOOP_END));
            scan++;
        }
        token->offset = scan + i - 1;
        (Tokens->data[i + scan - 1])->offset = i;
    }
}

// More complex loop unrolling

/*
        The following loops are unrolled into single operations:

                [->+<]
                    or
                        [-<+>]


                Multiplication Loops:
                MEM_MOV loops: 
        The optimizer takes a pointer to a list of tokens
        It will optimize the tokens and place them in a second list
        the original list will be freed from memory
        the pointer will now point to the new, optimized list
*/
void Optimizer(List_t* tokens);

// Lexer
List_t* Lexer(const char* p) {
    List_t* Tokens = Cons(25);

    size_t ip = 0;
    size_t len = strlen(p);
    while (ip < len) {
        Tok* t = malloc(sizeof(Tok));
        t->offset = 0;
        t->n = 1;

        switch (p[ip]) {
            case '+':
                t->flag = SUM;
                while (p[++ip] == '+') {
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
                // Via a basic non-consuming scan we can identify specific loop constructs and reduce them to single instructions
                // Reduce [-] and [+] loops to MEM_SET instructions
                if ((p[ip+1] == '-' || p[ip+1] == '+') && (p[ip+2] == ']')) {
                    t->flag = MEM_SET; t->n = 0; ip+=2;
                }
                else { t->flag = LOOP_START; }
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
        Append(Tokens, t);
    } 
    
    // Compute loop offsets
    Comp_Loops(Tokens);

    return Tokens;
}

// Basic interpreter
void nerv(const char* p) {
    List_t* tokens = Lexer(p);

    char mem[TAPE_LEN] = {0};
    
    char* ptr = mem;    // memory pointer
    size_t ip = 0;      // instruction pointer
    
    Tok* tmp;
    while ( ip < len(tokens) ) {
        tmp = tokens->data[ip];
        switch ( tmp->flag ) {
            case SUM:
                *ptr += tmp->n;
                break;
            case SUB:
                *ptr -= tmp->n;
                break;
            case SHR:
                ptr += tmp->n;
                break;
            case SHL:
                ptr -= tmp->n;
                break;
            case LOOP_START:
                if (!*ptr) {
                    ip = tmp->offset - 1;
                }
                break;
            case LOOP_END:
                if (*ptr) {
                    ip = tmp->offset - 1;
                }
                break;
            case IN:
                *ptr = getchar();
                break;
            case OUT:
                putchar(*ptr);
                break;
            case MEM_SET:
                *ptr = tmp->n;
                break;
            default:
                fprintf(stderr, "Unkown Token: { Flag: %d; Offset: %d; N: %d; } \n", tmp->flag, tmp->offset, tmp->n);
                exit(EXIT_FAILURE);
        }
    
        ++ip;
    }

    Destroy(tokens);
}

// BF -> C Compiler
void nervc(const char* p, const char* path) {
    FILE* out = fopen(path, "w");

    if (!out) {
        fprintf(stderr, "Compiler: Could not open %s \n", path);
        exit(EXIT_FAILURE);
    }
 
    size_t indent = 1; // Number of tabs for each line, starts at 1 for the main function

    List_t* tokens = Lexer(p);

    // Some basic necessities
    fprintf(out, "#include <stdio.h>\n\n\n\nint main(void) {\n\tchar mem[%d] = {0};\n\tchar* ptr = mem;\n", TAPE_LEN);
    for (size_t i = 0; i < len(tokens); ++i) {
        // first, indent
        Tok* t = tokens->data[i];
        char buffer[9999] = {0};

        for (int j = 0; j < indent - (t->flag == LOOP_END); ++j)
            fputc('\t', out);

        switch ( t->flag ) {
            case SUM:
                fprintf(out, "*ptr += %d;\n", t->n);
                break;
            case SUB:
                fprintf(out, "*ptr -= %d;\n", t->n);
                break;
            case SHR:
                fprintf(out, "ptr += %d;\n", t->n);
                break;
            case SHL:
                fprintf(out, "ptr -= %d;\n", t->n);
                break;
            case MEM_SET:
                fprintf(out, "*ptr = %d;\n", t->n);
                break;
            case LOOP_END:
                indent--;
                fputs("}\n", out);
                break;
            case OUT:
                fputs("putchar(*ptr);\n", out);
                break;
            case IN:
                fputs("*ptr = getchar();\n", out);
                break;
            case LOOP_START:
                indent++;
                fputs("while (*ptr) {\n", out);
                break;
        }
    }
    fprintf(out, "}");  // End of the main function
    fclose(out);
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

// Can interpret the tokens, or compile to C code to further optimize with the C compiler

int main(int argc, char* argv[]) {
    char str[999999];
    if (!Read_BF(argv[1], str)) {
        fprintf(stderr, "Could not read %s \n", argv[1]);
        exit(EXIT_FAILURE);
    }

    const char* prog = str;
    if (!validate_loops(prog)) {
        fprintf(stderr, "Invalid parens! \n");
        exit(EXIT_FAILURE);
    }
    nervc(prog, DEF_OUT);
    system("gcc -o out out.c -O3");

    clock_t t = clock();
    system("out");
    t = clock() - t;
    printf("Time taken to execute %s | %f \n", argv[1], (double)t / CLOCKS_PER_SEC);
}
