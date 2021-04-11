# cpplox

## Introduction

`cpplox` is a C++ implementation of `lox` from the [Crafting Interpreters](https://craftinginterpreters.com/) book.

## Dependencies

- [{fmt}](https://github.com/fmtlib/fmt) - a modern formatting library.

Only Linux is supported.

## Build

Run this from the project root directory:  
1. `mkdir build`  
2. `cd build`  
3. `cmake ..`  
4. `make cpplox`

## Features

Currently the following features are supported in `cpplox`:  

- Basic arithmetic operations  
- Logical operators
- Comparison operators
- Variables
- Line and block comments
- Shadowing
- Boolean constants - `true`, `false`
- `nil`
- Control flow - `while`, `if {} else {}`, `for`
- First-class functions
- Native functions written in C++
- Closures
- `print` statement
- Lambdas
- Classes
- Static methods
- Constructors
- Inheritance
- Modules
- Error formatting

## Examples

You can find examples in the [examples](./examples) folder.