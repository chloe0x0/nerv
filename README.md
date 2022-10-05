# nerv 🧠⚡

Nerv is an optimizing toolchain for interacting with Brainfuck programs. It includes an Interpreter, Visualizer, and Brainfuck to C compiler.

## usage
```console
> git clone https://github.com/chloe0x0/nerv.git
> cd nerv
> make
```

## testing
```console
make test
test.exe
```

## Optimizations
Nerv uses various optimization techniques to speed up the execution of brainfuck programs.

### Peephole Optimizations/ Run Length Encoding
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

#### Optimizations to add

### Precompute constant memory states
Consider the following Hello World program
```brainfuck
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.

Seeing that there is no , in the program
we can safeley pre-compute the state of the tape

in general, we can safeley pre-compute the state of the tape until we find
a user input
```
### Loop Unrolling
```brainfuck
loops such as 
[->+<]
[-<+>]

[->+>+>+<<<]

etc

can be compiled to constant time multiplications
```

## TODO

* More complex loop unrolling

* Build Visualizer

## Why bother with optimizations?
Brainfuck is a heinously innefficient language, and is rather easy to optimize.

## examples
examples is a directory containing lots of fun Brainfuck programs. A subfolder of examples, benchmarks, stores some intensive programs used to benchmark the interpreter and compiler.