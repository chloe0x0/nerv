#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "List.h"
#include "Token.h"
#include "Opt.h"
#include "nerv.h"

// Constants
#define USE_GETC 0
#define TAPE_LEN 90000
#define BUFFER_SIZE 4096 // num of bytes to read before writting to a file
#define CAP_OUT 1        // whether or not to output interpreter output to tmp.out
#define PASSES 2         // number of passes the optimizer will run

// Lookup table to print enum values as strings
const char *Flag_LT[11] = {"Sum", "Sub", "Loop_Start", "Loop_End", "SHR", "SHL", "OUT", "IN", "COM", "MEM_SET", "MUL"};

// Lookup table used by the Optimizer to tell if two tokens cancel one another out
// if the tokens cannot be canceled out, it stores the same token type
const Type CANCEL_LT[11] = {SUB, SUM, LOOP_START, LOOP_END, SHL, SHR, OUT, IN, COM, MEM_SET, MUL};

// Lookup table used by the Optimizer to convert token types to chars
// used for peephole optimization
const char OP_LT[11] = {'+', '-', '[', ']', '>', '<', '.', ',', ' ', ' ', ' '};

// basic pre-processing
void pproc(char* s)
{
    char* s_ = s;
    do
    {
        while (!strchr("><+-.,[]", *s_))
            ++s_;
    } while ((*s++ = *s_++));
}

/*
    Read a brainfuck file given a path and a buffer
    Returns a boolean indicating wether or not the file was succesfully read into the buffer
*/
bool Read_BF(const char *p, char *buff, size_t buffer_size)
{
    FILE *fp = fopen(p, "r");
    if (!fp)
    {
        return false;
    }

#if USE_GETC
    char c;
    while (!feof(fp))
    {
        c = fgetc(fp);
        if (c != ' ')
        {
            *buff++ = c;
        }
    }
    *buff = '\0';
#else
    char *program = NULL;

    if (fseek(fp, 0L, SEEK_END) == 0)
    {
        long size = ftell(fp);
        if (size == -1)
        {
            fprintf(stderr, "Error going to end of %s!\n", p);
            exit(EXIT_FAILURE);
        }

        program = malloc(sizeof(char) * size);
        if (!program)
        {
            fprintf(stderr, "Could not allocate memory for program\n");
            exit(EXIT_FAILURE);
        }

        if (fseek(fp, 0L, SEEK_SET) != 0)
        {
            fprintf(stderr, "Error going back to start of %s!\n", p);
            exit(EXIT_FAILURE);
        }

        // read the file into buffer
        size_t len = fread(program, sizeof(char), size + 1, fp);

        if (ferror(fp) != 0)
        {
            fprintf(stderr, "Error reading file to buffer from %s\n", p);
            exit(EXIT_FAILURE);
        }

        if (len > buffer_size)
        {
            fprintf(stderr, "Buffer size of %u is too small. Read %u chars!\n", buffer_size, len);
            exit(EXIT_FAILURE);
        }

        program[len++] = '\0';
        memcpy(buff, program, strlen(program) + 1);
        pproc(buff);
        free(program);
    }
    else
    {
        fclose(fp);
        return false;
    }
#endif

    fclose(fp);
    return true;
}


// Print list of tokens for debugging
void print_tokens(List_t *tokens, size_t start_, size_t end_)
{
    if ((start_ == end_) && end_ == 0)
        end_ = len(tokens);

    Tok *t;
    for (size_t i = start_; i < end_; ++i)
    {
        t = tokens->data[i];
        printf("Token %d | %s, n:= %d, offset:= %d\n", i, Flag_LT[t->flag], t->n, t->offset);
    }
}

// Helper function to ensure that incoming expressions have well formed loops
bool validate_loops(const char *prog)
{
    size_t l, r;
    l = r = 0;

    const char *tmp = prog;

    do
    {
        l += (*tmp == ']');
        r += (*tmp == '[');
    } while (*(tmp++) != '\0');

    return l == r;
}

// Convert Brainfuck Code to a set of Tokens
// Use of run length encoding to reduce redundancy and optimize the program
// A basic example:

/*
 *  +++++ would convert to a single Token of the following form
 *
 *  Token
 *      {
 *          flag = SUM,
 *          lexeme = +++++,
 *          n = 5
 *      }
 */
// In this way, rather than applying SUM to the current cell N times we simply add N to the cell once
// The asymptotic time complexity of c + n, where c is the current cell, goes from linear to constant
// The secondary optimization to start with will be storing loop offset locations in the token structures
// This eliminates the need to repeatedley scan back and forth to find matching tokens, an expensive and unnecessary computation

// Function to compute loop offsets
// An alternative method uses Lookup Tables
// The problem is that this often uses more space than necessary
// so just store loop jumps in the IR
// This optimization only aims to improve the Interpreter
/*

    [>>>++<<<]

    =>
      _________________________________
      v                               |
    ( [ ) | ( > ) | ( + ) | ( < ) | ( ] )
      |_______________________________^

*/
void Comp_Loops(List_t *Tokens)
{
    if (len(Tokens) == 0)
        return;

    for (size_t i = 0; i < len(Tokens); ++i)
    {
        Tok *token = Tokens->data[i];
        if (token->flag != LOOP_START)
            continue;

        // Scan ahead for next matching loop end token
        int count, scan;
        count = scan = 1;
        while (count)
        {
            assert(i + scan < len(Tokens));
            Type tmp = (Tokens->data[i + scan])->flag;
            count += (tmp == LOOP_START) - (tmp == LOOP_END);
            scan++;
        }
        token->offset = scan + i - 1;
        Tokens->data[i + scan - 1]->offset = i;
    }
}

// check if a loop returns to same cell after iterating
inline bool returns_to_start(List_t *tokens, Tok *loop, size_t start)
{
    size_t position = start;

    for (size_t ix = 1; ix < loop->offset-start; ++ix)
    {
        Tok* token = tokens->data[start + ix];
        position += (token->flag==SHR)*token->n - (token->flag==SHL)*token->n;
    }

    return position == start;
}

// id multiplication loops
bool is_mul(List_t *tokens, Tok *loop, size_t start)
{
    Tok *scn = tokens->data[start + 1];
    Tok *end = tokens->data[loop->offset-1];
    bool dec = (scn->flag==SUB && scn->n==1) || (end->flag==SUB && end->n==1);
    bool returns = returns_to_start(tokens, loop, start);
    bool moves = false;
    bool modifies = false;

    for (size_t ix = 1; ix < loop->offset - start; ++ix)
    {
        Tok *scn = tokens->data[start + ix];
        
        switch (scn->flag)
        {
            case SHR:
                moves = true;
                break;
            case SHL:
                moves = true;
                break;
            case SUM:
                modifies = true;
                break;
            case SUB:
                modifies = true;
                break;
            default:
                return false;
        }
    }

    return dec && returns && moves && modifies;
}

// More complex loop unrolling

/*
        The following loops are unrolled into single operations:

                [->+<] or [-<+>]


                Multiplication Loops:


        Synopsis:
            The optimizer takes a pointer to a list of tokens
            It will optimize the tokens and place them in a second list
            the original list will be freed from memory
            the pointer will now point to the new, optimized list



        Lets analyze the structure of a Multiplication Loop

        1: There must be a Sub(1) instruction at the start or end of the loop
        2:

*/
List_t *Optimizer(List_t *tokens)
{
    List_t *opt = Cons(250);

    Tok *t, *scn, *opt_tok, *loop, *unroll;
    t = scn = opt_tok = loop = unroll = NULL;

    bool canceled = false;

    // used when computing distance from start of loop
    // to end of loop during loop unrolling
    size_t dist;

    // used during loop unrolling
    int offset = 0;
    // multiple of cell val
    int mult = 0;

    for (size_t i = 0; i < len(tokens) - 1; ++i)
    {
        t = tokens->data[i];
        scn = tokens->data[i + 1];

        opt_tok = malloc(sizeof(Tok));
        memcpy(opt_tok, t, sizeof(Tok));
        
        // cancel out operations that 'undo' eachother
        canceled = (scn->flag == CANCEL_LT[t->flag]) && (scn->flag != t->flag);

        if (canceled)
        {
            // subtract the token's n field
            opt_tok->n -= scn->n;        

            if (opt_tok->n >= 0)
                scn->n = 0;
            else
            {
                scn->n = 0;
                opt_tok->flag = CANCEL_LT[opt_tok->flag];
                opt_tok->n = abs(opt_tok->n);
            }
        }

        switch (t->flag)
        {
            case LOOP_END:
                // the loop occurs directly at the end of the current one
                // it is therefore dead code and can be removed
                if (scn->flag == LOOP_START)
                {
                    i = scn->offset;
                    scn->n = 0;
                }
                break;
            case LOOP_START:
                // unroll loops
                // ensure that there is a sub(1) operation at the begining or end of 
                // the loop
                // loop examples
                /*
                    [->+<]
                    =>
                    

                */
                if (!is_mul(tokens, t, i))
                {
                    // check for MEM_SET
                    if ((scn->flag == SUB || scn->flag == SUM) && i+2==t->offset)
                    {
                        unroll = malloc(sizeof(Tok));
                        unroll->flag = MEM_SET;
                        unroll->n = 0;
                        unroll->offset = 0;

                        Append(opt, unroll);

                        i += 2;
                        scn->n = 0;
                        opt_tok->n = 0;
                    }
                    break;
                }

                // distance to end of loop
                dist = t->offset - i;

                for (size_t k = 1; k < dist-1; ++k)
                {
                    loop = tokens->data[i + k];

                    switch (loop->flag)
                    {
                        case SHR:
                            offset += loop->n;
                            break;
                        case SHL:
                            offset -= loop->n;
                            break;
                        case SUB:
                            if (k==1)
                                break;

                            mult -= loop->n;
                            unroll = malloc(sizeof(Tok));
                            unroll->flag = MUL;
                            unroll->offset = offset;
                            unroll->n = mult;

                            Append(opt, unroll);

                            mult = 0;
                            break;
                        case SUM:
                            mult += loop->n;
                            unroll = malloc(sizeof(Tok));
                            unroll->flag = MUL;
                            unroll->offset = offset;
                            unroll->n = mult;

                            Append(opt, unroll);

                            mult = 0;
                            break;
                        default:
                            break;
                    }

                }

                if (unroll->n)
                {
                    i = t->offset-1;
                    tokens->data[t->offset]->n = 0;
                    opt_tok->n = 0;
                    scn->n = 0;
                    offset = 0;

                    unroll = malloc(sizeof(Tok));
                    unroll->flag = MEM_SET;
                    unroll->offset = unroll->n = 0;

                    Append(opt, unroll);
                }
            
                break;
            default:
                break;
        }

        if (opt_tok->n || (opt_tok->flag == MEM_SET))
            Append(opt, opt_tok);
        else
            free(opt_tok);
    }

    opt_tok = malloc(sizeof(Tok));
    memcpy(opt_tok, scn, sizeof(Tok));
    if (opt_tok->n)
        Append(opt, opt_tok);

    Comp_Loops(opt);

    free(tokens);

    return opt;
}

// Lexer
/*

    Lexer converts a stream of characters into a list of tokens
    These tokens can be interpreted, or compiled into another language

    A few optimization steps can be easily done during the tokenization process, such as peephole optimization
    or compiling [-] | [+] loops to MEM_SET instructions

    Peephole optimizations:
        compile long runs of >, <, +, - into singular tokens

    args:
        p := program to tokenize
        opt := optimization level

*/
List_t *Lexer(const char *p, Opt opt)
{
    List_t *Tokens = Cons(50);

    size_t ip, ln;
    ip = 0;
    ln = strlen(p);

    Tok *t;

    char c;

    while (ip < ln)
    {
        t = malloc(sizeof(Tok));
        t->offset = 0;
        t->n = 1;

        c = p[ip];

        switch (c)
        {
            case '+':
                t->flag = SUM;
                break;
            case '-':
                t->flag = SUB;
                break;
            case '.':
                t->flag = OUT;
                break;
            case ',':
                t->flag = IN;
                break;
            case '>':
                t->flag = SHR;
                break;
            case '<':
                t->flag = SHL;
                break;
            case ']':
                t->flag = LOOP_END;
                break;
            case '[':
                t->flag = LOOP_START;
                break;
            default:
                t->flag = COM;
                break;
        }

        // perform peephole optimization if opt level greater than or equal to O1
        if (opt >= O1 && strchr("><+-", c))
        {
            while (p[++ip] == c)
                t->n++;
            ip--;
        }

        ip++;
        
        // Ignore other characters as comments
        if (t->flag == COM)
            free(t);
        else
            Append(Tokens, t);
    }

    // Compute loop offsets
    Comp_Loops(Tokens);

    // if opt level is O2, run the optimizer
    if (opt == O2) 
        return Optimizer(Tokens);       

    return Tokens;
}


// Basic interpreter
void nerv(const char *p, Opt o)
{
#if CAP_OUT
    FILE *output = fopen("./tmp.out", "w");

    if (!output)
    {
        fprintf(stderr, "Could not output to tmp.out!\n");
        exit(EXIT_FAILURE);
    }
#endif

    List_t *tokens = Lexer(p, o);

    char mem[TAPE_LEN] = {0};

    char *ptr = mem; // memory pointer
    size_t ip = 0;   // instruction pointer

    Tok *tmp;
    while (ip < len(tokens))
    {
        tmp = tokens->data[ip];
        switch (tmp->flag)
        {
            case SUM:
                *ptr += tmp->n;
                break;
            case SUB:
                *ptr -= tmp->n;
                break;
            case SHR:
                ptr += tmp->n;
                break;
            case SHL:
                ptr -= tmp->n;
                break;
            case LOOP_START:
                if (!*ptr)
                    ip = tmp->offset - 1;
                break;
            case LOOP_END:
                if (*ptr)
                    ip = tmp->offset - 1;
                break;
            case IN:
                *ptr = getchar();
                break;
            case OUT:
#if CAP_OUT
                fputc(*ptr, output);
#endif
                putchar(*ptr);
                break;
            case MEM_SET:
                *ptr = tmp->n;
                break;
            case MUL:
                *(ptr + tmp->offset) += *ptr * tmp->n;
                break;
            case COM:
                break;
            default:
                fprintf(stderr, "Unkown Token: { Flag: %d; Offset: %d; N: %d; } \n", tmp->flag, tmp->offset, tmp->n);
                exit(EXIT_FAILURE);
        }
        ++ip;
    }

#if CAP_OUT
    fclose(output);
#endif

    Destroy(tokens);
}

// BF -> C Compiler
void nervc(const char *p, const char *path, Opt o)
{
    FILE *out = fopen(path, "w");

    if (!out)
    {
        fprintf(stderr, "Compiler: Could not open %s \n", path);
        exit(EXIT_FAILURE);
    }

    size_t indent = 1; // Number of tabs for each line, starts at 1 for the main function

    List_t *tokens = Lexer(p, o);

    // Write chunks instead of calling fwrite for every token
    char buffer[BUFFER_SIZE] = {0};
    size_t buffer_len = 0;

    // Some basic necessities
    fprintf(out, "/* Generated by Nerv */\n#include <stdio.h>\n\nint main(void) {\n\tchar mem[%d] = {0};\n\tchar* ptr = mem;\n", TAPE_LEN);
    for (size_t i = 0; i < len(tokens); ++i)
    {
        Tok *t = tokens->data[i];

        if (buffer_len >= BUFFER_SIZE)
        {
            fwrite(buffer, 1, buffer_len, out);
            buffer_len = 0;
        }

        // write tabs to buffer
        for (size_t j = 0; j < indent - (t->flag == LOOP_END); ++j)
            buffer[buffer_len++] = '\t';

        switch (t->flag)
        {
            case SUM:
                buffer_len += sprintf(&buffer[buffer_len], "*ptr += %d;\n", t->n);
                break;
            case SUB:
                buffer_len += sprintf(&buffer[buffer_len], "*ptr -= %d;\n", t->n);
                break;
            case SHR:
                buffer_len += sprintf(&buffer[buffer_len], "ptr += %d;\n", t->n);
                break;
            case SHL:
                buffer_len += sprintf(&buffer[buffer_len], "ptr -= %d;\n", t->n);
                break;
            case MEM_SET:
                buffer_len += sprintf(&buffer[buffer_len], "*ptr = %d;\n", t->n);
                break;
            case LOOP_END:
                indent--;
                buffer[buffer_len++] = '}';
                buffer[buffer_len++] = '\n';
                break;
            case OUT:
                buffer_len += sprintf(&buffer[buffer_len], "putchar(*ptr);\n");
                break;
            case IN:
                buffer_len += sprintf(&buffer[buffer_len], "*ptr = getchar();\n");
                break;
            case LOOP_START:
                indent++;
                buffer_len += sprintf(&buffer[buffer_len], "while (*ptr) {\n");
                break;
            case MUL:
                buffer_len += sprintf(&buffer[buffer_len], "*(ptr + %d) += *ptr * %d;\n", t->offset, t->n);
                break;
            case COM:
                break;
        }
    }

    if (buffer_len > 0)
        fwrite(buffer, 1, buffer_len, out);

    fputs("}", out); // End of the main function
    fclose(out);
}