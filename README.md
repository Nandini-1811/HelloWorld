# HelloWorld Compiler

HelloWorld is a beginner-friendly educational programming language designed to be readable like plain English.  
This repository contains a full compiler pipeline and interpreter for `.learn` source files, implemented in C.

## Overview

HelloWorld was built with one core idea:

> Any complete beginner should be able to read a program and understand it instantly.

Instead of symbolic or cryptic syntax, HelloWorld uses readable keywords such as `let`, `define`, `returns`, `give`, `check`, `otherwise`, `repeat`, and `show`.

Example:

```learn
note: fibonacci in HelloWorld

define fib(n is num) returns num {
    check (n <= 1) {
        give n
    } otherwise {
        give fib(n - 1) + fib(n - 2)
    }
}

let result is num = fib(10)
show(result)
```

# Features
- English-like syntax for beginner readability
- .learn source file extension

## Primitive types:
- num
- decimal
- text
- bool
- nothing

## Core constructs:
- variable declarations
- reassignment
- functions
- return statements
- conditionals
- loops
- comments

## Compiler pipeline built from scratch in C
## Recursive function support in the interpreter

# Compiler Pipeline

## Stage 1 — Scanner

- Tokenizes .learn source code into a stream of tokens.

## Stage 2 — Parser

- Builds an Abstract Syntax Tree using recursive descent parsing.

## Stage 3 — Semantic Analyzer

- Performs type checking, symbol table management, and scope validation.

## Stage 4 — IR Generator

- Converts the AST into a 3-address intermediate representation.

## Stage 5 — Interpreter

- Executes the generated IR, including:
    - variable evaluation
    - arithmetic and comparisons
    - conditionals
    - loops
    - function calls
    - recursion

## Current Status
 -  Scanner                                 (done)
 -  Parser                                  (done)
 -  Semantic Analyzer                       (done)
 -  IR Generator                            (done)
 -  Interpreter                             (done)
 -  Code generation / bytecode backend      (in progress)


 ## Build

# Compile with GCC:

```bash
gcc -Wall -Iinclude src/scanner.c src/parser.c src/symtable.c src/semantic.c src/ir_gen.c src/interpreter.c src/main.c -o lang
```


## Run

- Run any .learn source file:

```bash
./lang test/hello.learn
```

On Windows PowerShell:
```bash
.\lang.exe test\hello.learn
```


# Example Output

- For the sample Fibonacci test program, the output is:

```bash
30
big number
0
1
2
3
4
55
```


## Project Structure

```bash
HelloWorld/
├── include/
│   ├── ast.h
│   ├── interpreter.h
│   ├── ir.h
│   └── tokens.h
├── src/
│   ├── scanner.c
│   ├── parser.c
│   ├── symtable.c
│   ├── semantic.c
│   ├── ir_gen.c
│   ├── interpreter.c
│   └── main.c
├── test/
│   ├── hello.learn
│   ├── test1.learn
│   ├── test2.learn
│   ├── test3.learn
│   └── test_errors.learn
├── Makefile
└── README.md
```


## Language Syntax

# Variable declaration

``` learn
let age is num = 20
let price is decimal = 99.99
let name is text = "Aryan"
let active is bool = true
```

# Reassignment
``` learn
age = 21
name = "Rahul"
```

# Functions
```learn
define add(a is num, b is num) returns num {
    give a + b
}
```

# Conditionals
```learn
check (age > 18) {
    show("adult")
} otherwise {
    show("minor")
}
```

# Loops
```learn
repeat (let i is num = 0 ; i < 5 ; i++) {
    show(i)
}
```

# Comments
```learn 
note: this is a comment
```

## Why this project matters

HelloWorld is not just a parser demo. It is a full educational language project that explores real compiler concepts:

- lexical analysis
- parsing
- AST design
- semantic analysis
- intermediate representations
- runtime execution
- function call handling
- recursion support


## Future Improvements
- list / map / pair / stack runtime support
- better error diagnostics
- standard library functions
- bytecode VM or native code generation
- improved CLI and test runner

## Author
Built by Nandini Goel as a compiler design and systems programming project in C.

## Repository Notes
If you run a test file, remember to include the correct path, for example:
```learn
.\lang.exe test\test1.learn
```