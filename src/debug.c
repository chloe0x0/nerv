#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "nerv.h"

#define TESTS 17

const char *tests[TESTS] = {"--++", "--+++", "++++++--[->+<]", "+++--", "+--", ">><<", "[->+<][+++++>+++++>+++>++<-]",
        "[->+<]", "[->++<]", "[->++>+<<]", "[>+<-]", "[-]", "[+]", "[->++>+++>++++<<<][-]+++--", "[->++>+<<<+>]",
        "[<<+>>-]", "[+++++++++.[-]+++++++++[<++++++++>-]]"};

void run(const char *p)
{
    printf("Tokenizing: %s\n", p);
    printf("-----------O0------------\n");
    print_tokens(Lexer(p, O0), 0, 0);
    printf("-----------O1------------\n");
    print_tokens(Lexer(p, O1), 0, 0);
    printf("-----------O2------------\n");
    print_tokens(Lexer(p, O2), 0, 0);

    printf("-----------Interpreting------------\n");
    nerv(p, O2);
    printf("-----------------------------------\n\n");
}

int main(void)
{   
    for (size_t i = 0; i < TESTS; ++i)
        run(tests[i]);
}