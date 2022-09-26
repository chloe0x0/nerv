#ifndef __NERV_H
#define __NERV_H

#include <stdbool.h>
#include "../include/List.h"
#include "../include/Opt.h"

// Read BF File to buffer
bool Read_BF(const char*, char*);
// Compute whether or not a program has valid parens
bool validate_loop(const char*);
// Compute loop jumps and store in the IR
void Comp_Loops(List_t*);
// Loop unrolling/ Dead Code Removal
void Optimizer(List_t*);
// Tokenizer/ Lexer
List_t* Lexer(const char*, Opt);
// interpreter
void nerv(const char*, Opt);
// Brainfuck to C compiler
void nervc(const char*, const char*, Opt);

#endif