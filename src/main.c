#include <stdio.h>
#include "nerv.h"

int main(void) {
    char buffer[500];
    Read_BF("examples/Hello.bf", buffer);

    nerv(buffer, O2);
}