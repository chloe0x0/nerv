#include <stdio.h>
#include <stdlib.h>
#include "nerv.h"

int main(void)
{
    const char *test = "+++--+[->+<]";

    printf("Tokenizing: %s\n", test);
    printf("-----------O0------------\n");
    print_tokens(Lexer(test, O0));
    printf("-----------O1------------\n");
    print_tokens(Lexer(test, O1));
    printf("-----------O2------------\n");
    print_tokens(Lexer(test, O2));

    nerv(test, O2);
}