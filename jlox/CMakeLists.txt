cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(jlox VERSION 1.0)

set(CLOX_SOURCES
    src/main.cpp;
    src/token.cpp;
    src/scanner.cpp;
    src/lox.cpp;
    src/expr.cpp;
    src/ast_printer.cpp;
    src/parser.cpp;
    src/interpreter.cpp;
    src/runtime_error.cpp
)

add_executable(${PROJECT_NAME} ${CLOX_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)
