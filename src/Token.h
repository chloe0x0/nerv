#ifndef __TOKEN_H_
#define __TOKEN_H_

typedef enum TOKEN_TYPE {
    SUM,            // Add x to the current cell
    SUB,            // Subtract x from the current cell
    LOOP_START,     // Mark the begining of a loop
    LOOP_END,       // Mark the end of a loop
    SHR,            // Shift the memory ptr to the right by x
    SHL,            // Shift the memory ptr to the left by x
    OUT,            // Print cell value as ASCII 
    IN,             // User I/O
    COM,            // Comment
    MEM_SET,        // Set the current cell value to x
    MOV_SUM,        // Add the current cell value to another cell by a given offset (Destructive to the current cell's value)
    MUL,            // Multiplication to a cell at a given offset by X*c where c is the current cell value
} TOKEN_TYPE;

typedef struct Token_t {
    TOKEN_TYPE flag;
    int n;              // number of times to apply the operation (computed by run length encoding)
    int offset;         // the position to offset the command
} Token_t;

#endif