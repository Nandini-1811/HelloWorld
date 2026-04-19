# HelloWorld Compiler

A compiler for **HelloWorld** — a custom educational programming language
built entirely from scratch in C. HelloWorld is designed so any complete
beginner can read a program and understand it instantly, with no
documentation needed.

Source files use the `.learn` extension.

---

## Quick Example
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
Output:
55

---

## Language Design

| Feature | Syntax |
|---|---|
| Variable | `let x is num = 10` |
| Reassign | `x = 20` |
| Function | `define add(a is num, b is num) returns num { }` |
| Return | `give a + b` |
| Print | `show(x)` |
| If / Else | `check (x > 5) { } otherwise { }` |
| Loop | `repeat (let i is num = 0 ; i < 10 ; i++) { }` |
| For-each | `repeat each item in scores { }` |
| Comment | `note: this is a comment` |

### Types
| Type | Meaning |
|---|---|
| `num` | whole integers |
| `decimal` | floating point numbers |
| `text` | strings |
| `bool` | true or false |
| `nothing` | void — functions that return no value |

---

## Compiler Architecture
```
HelloWorld is compiled in 5 stages:
Source (.learn)
│
▼
┌─────────────┐
│  Stage 1    │  scanner.c
│  Scanner    │  Reads characters → emits tokens
└──────┬──────┘
│  Token stream
▼
┌─────────────┐
│  Stage 2    │  parser.c
│  Parser     │  Tokens → Abstract Syntax Tree
└──────┬──────┘
│  AST
▼
┌─────────────┐
│  Stage 3    │  semantic.c + symtable.c
│  Semantic   │  Type checking + Symbol table
│  Analyzer   │
└──────┬──────┘
│  Validated AST
▼
┌─────────────┐
│  Stage 4    │  ir_gen.c
│  IR         │  AST → 3-address IR instructions
│  Generator  │
└──────┬──────┘
│  IR Program
▼
┌─────────────┐
│  Stage 5    │  interpreter.c
│  Interpreter│  Executes IR instructions directly
└─────────────┘
│
▼
Program Output
```
---

## Build Stages

| Stage | Status | Description |
|-------|--------|-------------|
| Stage 1 |  Complete | Scanner — tokenizes .learn source files |
| Stage 2 |  Complete | Parser — recursive descent, builds AST |
| Stage 3 |  Complete | Semantic Analyzer — type checking, symbol table |
| Stage 4 |  Complete | IR Generator — 3-address intermediate code |
| Stage 5 |  Complete | Interpreter — executes HelloWorld programs |

---

## Build

```bash
gcc -Wall -Iinclude src/scanner.c src/parser.c src/symtable.c src/semantic.c src/ir_gen.c src/interpreter.c src/main.c -o lang
```

## Run

```bash
.\lang.exe yourfile.learn
```

---

## Project Structure
```
HelloWorld/
├── include/
│   ├── tokens.h        # Token type definitions (Stage 1)
│   ├── ast.h           # AST node definitions (Stage 2)
│   ├── symtable.h      # Symbol table (Stage 3)
│   ├── ir.h            # IR instruction definitions (Stage 4)
│   └── interpreter.h   # Runtime value and frame types (Stage 5)
├── src/
│   ├── scanner.c       # Stage 1 — Lexical analyzer
│   ├── parser.c        # Stage 2 — Recursive descent parser
│   ├── symtable.c      # Stage 3 — Hash table symbol table
│   ├── semantic.c      # Stage 3 — Type checker
│   ├── ir_gen.c        # Stage 4 — AST to IR translator
│   ├── interpreter.c   # Stage 5 — IR execution engine
│   └── main.c          # Entry point — pipeline driver
├── test/
│   ├── test1.learn     # Variables and functions
│   ├── test2.learn     # Operators, decimals, loops
│   ├── test3.learn     # Comments
│   ├── test_errors.learn # Semantic error cases
│   └── hello.learn     # Full language showcase
└── README.md
```
---

## Error Detection

The compiler detects and reports:
- Semantic error: variable 'x' already declared in this scope
- Semantic error: undefined variable 'z'
- Semantic error: cannot assign decimal to variable 'price' of type num
- Semantic error: function 'add' expects 2 args but got 1

---

## Technical Highlights

- **Scanner** — hand-written DFA with maximal munch, tracks line/column
- **Parser** — recursive descent with operator precedence via call chain
- **Symbol table** — hash table with djb2-style hash, pointer-to-pointer scope exit
- **IR** — 3-address code with temporaries, labels, and jump instructions
- **Interpreter** — stack frame architecture for correct recursion

---

## Version Roadmap

### v1.0 — Complete 
Primitive types, functions, conditionals, loops, recursion,
full 5-stage compiler pipeline in C.

### v2.0 — Planned
- List, map, stack, pair data structures
- Full operation support per the language reference document
- Text slicing, joining, and indexing

---

## Author

Built by Nandini Goel — fresher CS project demonstrating compiler design
fundamentals including automata theory, recursive descent parsing,
type systems, intermediate representations, and runtime execution.

**GitHub:** github.com/Nandini-1811/HelloWorld