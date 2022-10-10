# nerv 🧠

Nerv is an optimizing Brainfuck Interpreter and Brainfuck to C Compiler. There is a bug when running SkipLoop.bf, but Ill get around 
to it another time.

## usage

Running the interpreter
```
nerv <Path to File> <Optimization flag: -O0, -O1, or -O2>

----------------------------------------
O0: No Optimizations
O1: Peephole Optimizations
O2: Dead code removal and loop unrolling
----------------------------------------
```

### Build and run Hello World
```console
> git clone https://github.com/chloe0x0/nerv.git
> cd nerv
> make
> nerv examples/Hello.bf -O2
Hello World!
```

## testing
```console
> make test
> test.exe
```

## Optimizations
Nerv uses various optimization techniques to speed up the execution of brainfuck programs.

### Peephole Optimizations
Sequences of +, -, <, > are compiled into single tokens
```brainfuck
+++++++++++++

The lexer will convert the above program to a single token
that tells the interpreter/ compiler to add 13 to the current cell
in this way, instead of 13 tokens that add 1 each time
we just use a single token
the same idea is applied to 

- 
<
and
>

```

### Common Loop Idioms
```brainfuck
[-] and [+]
get compiled into single instructions
which set the memory cell value to 0
```

### Dead code removal

operations that undo eachother get reduced
```brainfuck
>-++
gets compiled to 
>+

>>><<<
both tokens get removed, as they undo eachother


[->+<][->>+++++++>+++<<<]
the second loop is removed, as it can never be entered
```

### Loop Unrolling
```brainfuck
loops such as 
[->+<]
[-<+>]

[->+>+>+<<<]

etc

are compiled to constant time multiplications
```

given 
```brainfuck
[->++>+++<<]
```

on O2 optimizations the BF to C compiler will generate

```c
*(ptr + 1) += *ptr * 2;
*(ptr + 2) += *ptr * 3;
*ptr = 0;
```

without unrolling it would generate

```C
while (*ptr) {
    *ptr -= 1;
    ptr += 1;
    *ptr += 2;
    ptr += 1;
    *ptr += 3;
    ptr -= 2;
}
```

#### Optimizations to add

### Speculative Execution 
Consider the following Hello World program
```brainfuck
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.

Seeing that there is no , in the program
we can safeley pre-compute the state of the tape

in general, we can safeley pre-compute the state of the tape until we find
a user input
```

## TODO

* Build Visualizer

* Speculative Execution

## Why bother with optimizations?
Brainfuck is a heinously innefficient language, and is rather easy to optimize.

## examples
examples is a directory containing lots of fun Brainfuck programs. A subfolder of examples, benchmarks, stores some intensive programs used to benchmark the interpreter and compiler.