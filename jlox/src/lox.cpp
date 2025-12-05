#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <lox.hpp>
#include <scanner.hpp>

bool HAD_ERROR = false;

void Lox::run(std::string source)
{
    if (HAD_ERROR)
    {
        exit(65);
    }

    Scanner scanner{ source };
    auto tokens = scanner.scanTokens();

    for (auto &token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}

void Lox::runPrompt()
{
    std::string line;
    std::cout << "-- lox interpreter interactive shell --" << std::endl;

    for (;;)
    {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line.empty())
        {
            break;
        }
        else if (line == ".exit")
        {
            break;
        }

        run(line);

        if (HAD_ERROR)
        {
            HAD_ERROR = false;
        }
    }
}

void Lox::runFile(const char* filename_ptr)
{
    std::filesystem::path filepath(std::string{ filename_ptr });

    std::ifstream inputFile;
    inputFile.open(filepath, std::ios_base::binary);

    if (inputFile.fail())
    {
        exit(1);
    }

    std::stringstream sstream;
    sstream << inputFile.rdbuf();

    inputFile.close();

    std::string content = sstream.str();
    run(content);
}

void Lox::report(int line, std::string where, std::string message)
{
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    HAD_ERROR = true;
}

void Lox::error(int line, std::string message)
{
    report(line, "", message);
}
