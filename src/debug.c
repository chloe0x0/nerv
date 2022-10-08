#include <stdio.h>
#include <stdlib.h>
#include "nerv.h"

#define TESTS 12

const char *tests[TESTS] = {"--++", "--+++", "++++++--[->+<]", "+++--", "+--", ">><<", "[->+<][+++++>+++++>+++>++<-]",
        "[->+<]", "[->++<]", "[->++>+<<]", "[>+<-]", "[>++>+++>+<<<-]"};

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
    char buffer[200] = {0}; 

    for (size_t i = 0; i < TESTS; ++i)
        run(tests[i]);
    
    Read_BF("examples/Hello.bf", buffer, 200);
    printf("%s\n", buffer);
    print_tokens(Lexer(buffer, O2), 0, 0);

    print_tokens(Lexer(buffer, O1), 0, 0);    
}