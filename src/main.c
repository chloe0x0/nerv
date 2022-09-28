#include <stdio.h>
#include <stdlib.h>
#include "nerv.h"

int main(void)
{
    char buffer[9000];
    if (!Read_BF("examples/benchmarks/bitwidth.bf", buffer, 9000))
    {
        fprintf(stderr, "Could not open %s!\n", buffer);
        exit(EXIT_FAILURE);
    }

    nerv(buffer, O2);
}