# Brainfuck 

Brainfuck is a Turing Complete esoteric programming language made by Urban Müller in 1993. The language operates on an infinite tape of discrete memory cells where each cell contains some number of bytes. 

The number of cells on the tape, the bytes in each cell, how to handle the pointer going off either end of the tape, or cell value overflows, are all left up to the implementor. By convention, 30,000 8 bit cells are used.

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
char mem[30000] = {0};  // tape of 30,000 8 bit memory cells 
char* ptr = mem;        // memory pointer, intially pointing at the first cell
```

Lets parse through a simple program

```brainfuck
+++++++[->+<]
```

Initialize the tape and memory pointer

```
+---------------------------+   
| 0 | 0 | 0 | 0 | 0 | 0 | 0 |   ...
+---------------------------+   
 ^
ptr
```

the first instruction tells us to add one to the cell

lets do that

```
+---------------------------+   
| 1 | 0 | 0 | 0 | 0 | 0 | 0 |   ...
+---------------------------+   
 ^
ptr
```

lets skip to the interesting part

```
+---------------------------+   
| 7 | 0 | 0 | 0 | 0 | 0 | 0 |   ...
+---------------------------+   
 ^
ptr
```

We now encounter a [ which indicates the begining of a loop.

In brainfuck a loop is only entered if the current cell has a nonzero value. In other words, we loop until the currenct cell is 0.


Since the current cell has a nonzero value, we enter the loop. The loop body tells us to subtract 1 from the current cell, move the cell pointer 1 cell to the right, add 1 to the cell, then move the cell pointer 1 cell to the left. 


```
+---------------------------+   
| 6 | 0 | 0 | 0 | 0 | 0 | 0 |   ...
+---------------------------+   
      ^
ptr
```

```
+---------------------------+   
| 6 | 1 | 0 | 0 | 0 | 0 | 0 |   ...
+---------------------------+   
      ^
ptr
```

```
+---------------------------+   
| 6 | 1 | 0 | 0 | 0 | 0 | 0 |   ...
+---------------------------+   
  ^
ptr
```

We have added one to the rightmost cell, and moved back to our original cell

The loop is iterated once more as the cell we point at has a value of 6

skipping till the end

```
+---------------------------+   
| 0 | 7 | 0 | 0 | 0 | 0 | 0 |   ...
+---------------------------+   
  ^
ptr
```

phew

We have succesfully moved the 7 one cell to the right! And to think it only took 7 iterations of a loop.

Generally, moving a cell's value to the left or right will take N iterations where N is the cell's value.

This loop is a common bf idiom

```brainfuck
[->+<]
```

It adds the current cells value once cell to the right, and sets the cell to 0 after the move.

By adding more > and < instructions you can perform this operation on cells any distance away from the current cell.

Through reversing their order 

```brainfuck
[-<+>]
```

it instead does the same operation, just on the leftmost cell rather than the rightmost cell


Through adding more + instructions we get a multiplication loop.

```brainfuck
[->++<]
```

will add 2 times the current cell value to the rightmost cell, and otherwise have the same behavior.


## Good Resources

### Visualizers
A good visualizer can be useful when learning Brainfuck. 

* https://ashupk.github.io/Brainfuck/brainfuck-visualizer-master/index.html

* https://brainfuck-visualizer.herokuapp.com/

### Reading

* https://esolangs.org/wiki/Brainfuck

* https://en.wikipedia.org/wiki/Brainfuck

## Fireship, my beloved

* https://www.youtube.com/watch?v=hdHjjBS4cs8&ab_channel=Fireship