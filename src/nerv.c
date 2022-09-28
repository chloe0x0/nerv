#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "List.h"
#include "Token.h"
#include "Opt.h"
#include "nerv.h"

// Constants
#define USE_GETC 0
#define TAPE_LEN 90000
#define BUFFER_SIZE 4096 // num of bytes to read before writting to a file
#define CAP_OUT 1        // whether or not to output interpreter output to tmp.out

/*
    Read a brainfuck file given a path and a buffer
    Returns a boolean indicating wether or not the file was succesfully read into the buffer
*/
bool Read_BF(const char* p, char* buff, size_t buffer_size) {
    FILE* fp = fopen(p, "r");
    if (!fp) {
        return false;
    }

#if USE_GETC
        char c;
        while ( !feof(fp) ) {
            c = fgetc(fp);
            if (c != ' ') { *buff++ = c; }
        }
        *buff = '\0';
#else
        char* program = NULL;

        if (fseek(fp, 0L, SEEK_END) == 0) {
            long size = ftell(fp);
            if (size == -1) {
                fprintf(stderr, "Error going to end of %s!\n", p);
                exit(EXIT_FAILURE);
            }

            program = malloc(sizeof(char) * size);
            if (!program) { 
                fprintf(stderr, "Could not allocate memory for program\n");
                exit(EXIT_FAILURE);
            }

            if (fseek(fp, 0L, SEEK_SET) != 0) {
                fprintf(stderr, "Error going back to start of %s!\n", p);
                exit(EXIT_FAILURE);
            }

            // read the file into buffer
            size_t len = fread(program, sizeof(char), size+1, fp);
        
            if (ferror(fp) != 0) {
                fprintf(stderr, "Error reading file to buffer from %s\n", p);
                exit(EXIT_FAILURE);
            }

            if (len > buffer_size) {
                fprintf(stderr, "Buffer size of %u is too small. Read %u chars!\n", buffer_size, len);
                exit(EXIT_FAILURE);
            }

            program[len++] = '\0';
            memcpy(buff, program, strlen(program) + 1);
            free(program);
        } else {
            return false;
        }
#endif

    return true;
}

// Helper function to ensure that incoming expressions have well formed loops
bool validate_loops(const char* prog) {
    size_t l, r;
    l = r = 0;

    const char* tmp = prog;

    do {
        l += (*tmp == ']');
        r += (*tmp == '[');
    } while(*(tmp++) != '\0');

    return l == r;
}

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
// so just store loop jumps in the IR
// This optimization only aims to improve the Interpreter
/*

    [>>>++<<<]

    =>
      _________________________________
      v                                |  
    ( [ ) | ( > ) | ( + ) | ( < ) | ( ] )
      |______________________________^

*/
void Comp_Loops(List_t* Tokens) {
    for (size_t i = 0; i < len(Tokens); ++i) {
        Tok* token = Tokens->data[i];
        if (token->flag != LOOP_START) { continue; }
        
        // Scan ahead for next matching loop end token
        int count, scan;
        count = scan = 1;
        while (count) {
            TOKEN_TYPE tmp = (Tokens->data[i + scan])->flag;
            count += (tmp == LOOP_START) - (tmp==LOOP_END);
            scan++;
        }
        token->offset = scan + i - 1;
        Tokens->data[i + scan - 1]->offset = i;
    }
}

// More complex loop unrolling

/*
        The following loops are unrolled into single operations:

                [->+<] or [-<+>]


                Multiplication Loops:
        
        
        Synopsis:
            The optimizer takes a pointer to a list of tokens
            It will optimize the tokens and place them in a second list
            the original list will be freed from memory
            the pointer will now point to the new, optimized list



        Lets analyze the structure of a Multiplication Loop

        1: There must be a Sub(1) instruction at the start or end of the loop
        2: 

*/
void Optimizer(List_t* tokens);

// Lexer
/*

    Lexer converts a stream of characters into a list of tokens
    These tokens can be interpreted, or compiled into another language

    A few optimization steps can be easily done during the tokenization process, such as peephole optimization
    or compiling [-] | [+] loops to MEM_SET instructions 

    Peephole optimizations:
        compile long runs of >, <, +, - into singular tokens

    args:
        p := program to tokenize
        opt := optimization level

*/
List_t* Lexer(const char* p, Opt opt) {
    List_t* Tokens = Cons(50);

    size_t ip, ln;
    ip = 0;
    ln = strlen(p);

    Tok* t;
    
    char c;

    while (ip < ln) {
        t = malloc(sizeof(Tok));
        t->offset = 0;
        t->n = 1;

        c = p[ip];

        switch (c) {
            case '+': 
                t->flag = SUM;        
                break;
            case '-':
                t->flag = SUB;
                break;
            case '.':
                t->flag = OUT;
                break;
            case ',':
                t->flag = IN; 
                break;
            case '>':
                t->flag = SHR;
                break;
            case '<':
                t->flag = SHL;
                break;
            case ']':
                t->flag = LOOP_END;
                break;
            case '[':
                // Via a basic non-consuming scan we can identify specific loop constructs and reduce them to single instructions
                // Reduce [-] and [+] loops to MEM_SET instructions
                if (opt != O2) { t->flag = LOOP_START; }
                else {
                    if ((p[ip+1] == '-' || p[ip+1] == '+') && (p[ip+2] == ']')) {
                        t->flag = MEM_SET; 
                        t->n = 0; 
                        ip+=2;
                    } else {
                        t->flag = LOOP_START;
                    }
                }
                break;
            default: 
                t->flag = COM;  
                break;
        }

        // perform peephole optimization if opt level greater than or equal to O1
        if (opt >= O1) {
            if (strchr("><+-", c)) {
                while (p[++ip] == c) {
                    t->n++;
                }
                ip--;
            }
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
void nerv(const char* p, Opt o) {
#if CAP_OUT
    FILE* output = fopen("./tmp.out", "w");

    if (!output) {
        fprintf(stderr, "Could not output to tmp.out!\n");
        exit(EXIT_FAILURE);
    }
#endif

    List_t* tokens = Lexer(p, o);

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
#if CAP_OUT
                fputc(*ptr, output);
#endif
                putchar(*ptr);
                break;
            case MEM_SET:
                *ptr = tmp->n;
                break;
            case COM:
                break;
            default:
                fprintf(stderr, "Unkown Token: { Flag: %d; Offset: %d; N: %d; } \n", tmp->flag, tmp->offset, tmp->n);
                exit(EXIT_FAILURE);
        }
    
        ++ip;
    }

#if CAP_OUT
    fclose(output);
#endif

    Destroy(tokens);
}

// BF -> C Compiler
void nervc(const char* p, const char* path, Opt o) {
    FILE* out = fopen(path, "w");

    if (!out) {
        fprintf(stderr, "Compiler: Could not open %s \n", path);
        exit(EXIT_FAILURE);
    }
 
    size_t indent = 1; // Number of tabs for each line, starts at 1 for the main function

    List_t* tokens = Lexer(p, o);

    // Write chunks instead of calling fwrite for every token
    char buffer[BUFFER_SIZE] = {0};
    size_t buffer_len = 0;

    // Some basic necessities
    fprintf(out, "/* Generated by Nerv */\n#include <stdio.h>\n\nint main(void) {\n\tchar mem[%d] = {0};\n\tchar* ptr = mem;\n", TAPE_LEN);
    for (size_t i = 0; i < len(tokens); ++i) {
        Tok* t = tokens->data[i];

        if (buffer_len >= BUFFER_SIZE) {
            fwrite(buffer, 1, buffer_len, out);
            buffer_len = 0;
        }

        // write tabs to buffer
        for (int j = 0; j < indent - (t->flag == LOOP_END); ++j)
            buffer[buffer_len++] = '\t';

        switch ( t->flag ) {
            case SUM:
                buffer_len += sprintf(&buffer[buffer_len], "*ptr += %d;\n", t->n);
                break;
            case SUB:
                buffer_len += sprintf(&buffer[buffer_len], "*ptr -= %d;\n", t->n);
                break;
            case SHR:
                buffer_len += sprintf(&buffer[buffer_len], "ptr += %d;\n", t->n);
                break;
            case SHL:
                buffer_len += sprintf(&buffer[buffer_len], "ptr -= %d;\n", t->n);
                break;
            case MEM_SET:
                buffer_len += sprintf(&buffer[buffer_len], "*ptr = %d;\n", t->n);
                break;
            case LOOP_END:
                indent--;
                buffer[buffer_len++] = '}';
                buffer[buffer_len++] = '\n';
                break;
            case OUT:
                buffer_len += sprintf(&buffer[buffer_len], "putchar(*ptr);\n");
                break;
            case IN:
                buffer_len += sprintf(&buffer[buffer_len], "*ptr = getchar();\n");
                break;
            case LOOP_START:
                indent++;
                buffer_len += sprintf(&buffer[buffer_len], "while (*ptr) {\n"); 
                break;
            case COM:
                break;
        }
    }
 
    if (buffer_len > 0) {
        fwrite(buffer, 1, buffer_len, out);
    }

    fputs("}", out);  // End of the main function
    fclose(out);
}