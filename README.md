# A reimplementation of the Lox interpreters in the Crafting Interpreters book by Robert Nystrom 

## Overview

This repository contains 2 main implementation stored in 2 directories:

1. The `jlox` directory stores the C++ implementation to the Tree Walk Interpreter for Lox
2. The `clox` directory stores the C reimplementation to the Bytecode Virtual Machine for Lox *(work in progress)* 

### JLox: The Tree Walk Interpreter

The Tree Walk Interpreter for Lox was originally implemented in Java. In this version, I translated the interpreter into C++, which means I have to add some additional functionalities to ensure no memory leaks happen since C++ doesn't have a native garbage collector. Unfortunately, this modification also makes the C++ code NOT one-to-one translatable to the original Java version.

**Note: This repository also contains my answers and additional implementations for the challenges inside the book.**
