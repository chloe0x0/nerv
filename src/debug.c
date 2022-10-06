#include <stdio.h>
#include <stdlib.h>
#include "nerv.h"

#define TESTS 6
#define FILES 1

const char *tests[TESTS] = {"--++", "--+++", "++++++--[->+<]", "+++--", "+--", ">><<"};

const char *files[FILES] = {"examples/Hello.bf"};

void run(const char *p)
{
    printf("Tokenizing: %s\n", p);
    printf("-----------O0------------\n");
    print_tokens(Lexer(p, O0));
    printf("-----------O1------------\n");
    print_tokens(Lexer(p, O1));
    printf("-----------O2------------\n");
    print_tokens(Lexer(p, O2));

    printf("-----------Interpreting------------\n");
    nerv(p, O2);
    printf("-----------------------------------\n\n");
}

int main(void)
{
    char buffer[9000];

    for (size_t i = 0; i < TESTS; ++i)
        run(tests[i]);

    printf("---------------------------------\n");
    printf("Testing files\n");
    
    for (size_t i = 0; i < FILES; ++i)
    {
        if (!Read_BF(files[i], buffer, 147))
        {
            fprintf(stderr, "Could not open %s\n", files[i]);
            exit(EXIT_FAILURE);
        }

        run(buffer);
    }
}