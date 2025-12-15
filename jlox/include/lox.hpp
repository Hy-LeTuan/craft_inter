#pragma once

#include <interpreter.hpp>
#include <runtime_error.hpp>
#include <string>
#include <token.hpp>

extern bool HAD_ERROR;
extern bool HAD_RUNTIME_ERROR;

class Lox
{
  public:
    static void run(std::string source);
    static void runPrompt();
    static void runFile(const char* filename_ptr);

    static void runtimeError(RuntimeError error);
    static void report(int line, std::string where, std::string message);
    static void error(int line, std::string message);
    static void error(Token* token, std::string message);

  private:
    static Interpreter interpreter;
};
