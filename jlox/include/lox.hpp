#pragma once
#include <string>

class Lox
{
  public:
    static void run(std::string source);
    static void runPrompt();
    static void runFile(const char* filename_ptr);
    static void report(int line, std::string where, std::string message);
    static void error(int line, std::string message);
};
