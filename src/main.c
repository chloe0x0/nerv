#include <stdio.h>
#include <stdlib.h>
#include "nerv.h"

int main(void) {
    char buffer[500];
    if (!Read_BF("examples/Hello.bf", buffer, 500)) {
        fprintf(stderr, "Could not open %s!\n", buffer);
        exit(EXIT_FAILURE);
    }

    nerv(buffer, O2);
}