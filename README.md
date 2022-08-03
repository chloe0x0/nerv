# nerv

A fast, efficient Brainfuck compiler in C

## Description of the project
Nerv is an optimizing toolchain for interacting with Brainfuck programs. It includes an Interpreter, Visualizer, and Brainfuck to C compiler.

## Optimizations
Nerv's optimizations are all done in its Lexer during Tokenization. Nerv will reduce expressions with Run Length Encoding of the basic operations
```bf 
+ - > <
``` 
For instance, the following program
```bf
+++++
```
would be converted to a single Token of the form
```
        SUM {
            N = 5;
            ...
        }
```
In this way, rather than adding 1 to a cell 5 times we can simply add the Token's N field. Therefore addition and subtraction by N are constant time operations rather than linear ones. 

Nerv also precomputes the loop jumps, encoding them in the tokens themselves. This only has an impact on the Interpreter, though it is a noteworthy optimization
Known Loop structures get unrolled into simple operations
```bf
[-]
```
will be compiled to a single MEM_SET operation, eliminating the loop