#include <iostream>
#include <lox.hpp>

int main(const int argc, const char** argv)
{
    if (argc > 2)
    {
        std::cout << "Usage: jlox [script]" << std::endl;
        return 64;
    }
    if (argc == 2)
    {
        Lox::runFile(argv[1]);
    }
    else if (argc == 1)
    {
        Lox::runPrompt();
    }

    return 0;
}
