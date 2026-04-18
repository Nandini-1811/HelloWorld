# HelloWorld Programming Language 

HelloWorld is a beginner-friendly programming language designed to be **read like plain English**.  
It is implemented from scratch in C with a complete compiler pipeline and interpreter.

---

## Motivation

Most programming languages are difficult for beginners because of symbols and syntax complexity.

HelloWorld solves this by using:
- plain English keywords
- readable sentence-like syntax
- explicit typing

Example:

```learn
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

## Features

- English-like readable syntax  
- `.learn` file extension  
- Built entirely in C  

### Supports:
- variables and reassignment  
- functions  
- recursion  
- conditionals (`check / otherwise`)  
- loops (`repeat`)  
- printing (`show`)  

- Full compiler pipeline implemented  
- Working interpreter with recursion support  

## Compiler Architecture

The project follows a real-world compiler design:

- Stage 1 — Scanner (Lexer)
        
    Converts source code → tokens

- Stage 2 — Parser
    
     Builds Abstract Syntax Tree (AST)

     Recursive descent parsing

- Stage 3 — Semantic Analysis

    Type checking

    Symbol table

    Scope validation
- Stage 4 — IR Generation

    Converts AST → 3-address code
- Stage 5 — Interpreter

    Executes IR 
    
    Handles:
        
        control flow
            
        function calls
            
        recursion
            
        variable storage


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

## Build
Using GCC
```bash
gcc -Wall -Iinclude src/scanner.c src/parser.c src/symtable.c src/semantic.c src/ir_gen.c src/interpreter.c src/main.c -o lang
```

## Run
- Linux / Mac
```learn
./lang test/hello.learn
```

- Windows (PowerShell)
``` learn 
.\lang.exe test\hello.learn
```

Always provide correct file path (files are inside test/ folder)

# Example Output
```learn
30
big number
0
1
2
3
4
55
```

# Language Syntax

- Variable Declaration
```learn
let x is num = 10
let name is text = "Aryan"
```

- Reassignment
```learn
x = 20
```

- Function
```learn
define add(a is num, b is num) returns num {
    give a + b
}
```
- Conditional
```learn
check (x > 5) {
    show("big")
} otherwise {
    show("small")
}
```

- Loop
```learn
repeat (let i is num = 0 ; i < 5 ; i++) {
    show(i)
}
```
- Comment
```learn
note: this is a comment
```

# Current Status
 - Scanner
 - Parser
 - Semantic Analyzer
 - IR Generator
 - Interpreter
 - Code Generation (Planned)


# Future Improvements
 Support for:
- lists
- maps
- stack operations
- Better error handling
- CLI improvements
- Bytecode VM / Assembly backend


# Why this project stands out

This is not a simple parser project.

It demonstrates:

- Compiler design concepts
- AST construction
- Intermediate Representation (IR)
- Runtime execution engine
- Function call handling with recursion
- Scope and memory management

# Author

Nandini Goel
BTech CS | Compiler Design & Systems Programming Enthusiast

GitHub: https://github.com/Nandini-1811