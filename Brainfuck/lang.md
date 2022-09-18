# Brainfuck

Brainfuck is a Turing Complete esoteric programming language made by Urban Müller in 1993. The language operates on an infinite tape of discrete memory cells where each cell contains a single byte. 

The 

The tape is modified by way of a memory pointer which can increment/ decrement the value of the cell it is pointing at, and move one cell to the left or right. The memory pointer is often positioned in the middle of the tape 

Its difficulty largley stems from its extreme minimalism (the entire language has only 8 commands). 

Below is a table of each brainfuck instruction and the corresponding C code.

| Brainfuck | C equivalent      |
|-----------|-------------------|
|   +       |     *ptr++;       |
|   -       |     *ptr--;       |
|   >       |      ptr++;       |
|   <       |      ptr--;       |
|   [       |while(*ptr) {      |
|   ]       |            }      |
|   .       |putchar(*ptr);     |
|   ,       |*ptr = getchar();  |

The brainfuck program initialization in C looks like:

```c
char mem[30000] = {0};  // tape of 30,000 memory cells 
char* ptr = mem;        
```

A simple Cat program in BF looks like this:

```brainfuck
,[.,]
```