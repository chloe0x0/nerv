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
    IN,             // Read from stdin
    COM,            // Comment
    MEM_SET,        // Set the current cell value to x
} TOKEN_TYPE;

// Brainfuck Token structure
typedef struct Tok {
    TOKEN_TYPE flag;   // enumerated type representing the type of instruction the token encodes
    size_t n;          // number of times to apply the operation (computed by run length encoding)
    int offset;        // the position to offset the command
                       // in the event that the token is a loop it is the position to jump to during looping
} Tok;

#endif
