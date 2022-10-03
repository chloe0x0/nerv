#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "nerv.h"

#define BENCH_PATH "./examples/benchmarks/"
#define BUFF_SIZE 90000
#define OUT_SIZE 100
#define BN 5

const char *benchmarks[BN] = {
    "./examples/benchmarks/Bench.bf", 
    "./examples/benchmarks/bitwidth.bf", 
    "./examples/benchmarks/easy-opt.bf", 
    "./examples/benchmarks/Skiploop.bf", 
    "./examples/benchmarks/too_slow.bf"
};

const char *bench_outs[BN] = {
    "./examples/benchmarks/Bench.out", 
    "./examples/benchmarks/bitwidth.out", 
    "./examples/benchmarks/easy-opt.out", 
    "./examples/benchmarks/Skiploop.out", 
    "./examples/benchmarks/too_slow.out"
};

// Iterate through benchmarks and return number of correct outputs
int test_interpreter(void)
{
    int correct = 0;

    // test interpreter
    char buff[BUFF_SIZE], exp[OUT_SIZE], out[OUT_SIZE];

    for (int i = 0; i < BN; ++i)
    {
        const char *path = benchmarks[i];
        const char *out_path = bench_outs[i];

        if (!Read_BF(path, buff, BUFF_SIZE))
        {
            fprintf(stderr, "Could not read: %s\n", path);
            exit(EXIT_FAILURE);
        }

        printf("%s\t", path);
        nerv(buff, O2);

        // Read in expected output and actual output
        if (!Read_BF("./tmp.out", out, OUT_SIZE))
        {
            fprintf(stderr, "Could not read tmp.out");
            exit(EXIT_FAILURE);
        }
        Read_BF(out_path, exp, OUT_SIZE);

        // Check if expected output matches actual output from interpreter
        if (!strcmp(out, exp))
        {
            correct++;
            printf("\n\tCorrect Output!\n");
        }
        else
        {
            printf("\n\tInccorect Output!\n");
            printf("\nExpected: {%s}", exp);
        }

        putc('\n', stdout);
    }

    return correct;
}

int main(void)
{
    printf("Testing Interpreter!\n\n");
    int correct = test_interpreter();
    printf("%.2f%% correct.\n", ((float)correct / (float)BN) * 100);
}