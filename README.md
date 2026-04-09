# LightCompiler

A single-pass lexical, syntax, and semantic analyzer implementing compilation and automate principles.

## Features
- Lexical analysis (identifiers, numbers, operators, keywords)
- Syntax validation (LL(1) recursive descent parsing)
- Semantic checks (variable declaration before use, type consistency)

## Supported Constructs
- `program ... var ... begin ... end`
- Variable declarations (`integer` only)
- Assignments (`:=`)
- Arithmetic expressions (`+ - * /`)
- Relational conditions (`< > =`)
- `if ... then ... endif`
- `readln()` and `writeln()`

## Build & Run
```bash
gcc main.c -o pascal_analyzer
./pascal_analyzer
