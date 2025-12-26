#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <lox.hpp>
#include <scanner.hpp>
#include <parser.hpp>
#include <ast_printer.hpp>

bool HAD_ERROR = false;
bool HAD_RUNTIME_ERROR = false;

Interpreter Lox::interpreter = Interpreter{};

void Lox::run(std::string source)
{
    if (HAD_ERROR)
    {
        exit(65);
    }

    Scanner scanner{ source };
    auto tokens = scanner.scanTokens();

    Parser parser{ std::move(tokens) };
    std::vector<stmt::Stmt*> statements = parser.parse();

    if (HAD_ERROR)
    {
        return;
    }

    Lox::interpreter.interpret(statements);

    // // top down clean up of statements
    for (auto statement : statements)
    {
        if (statement)
        {
            delete statement;
        }
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

        if (line.find(";") == std::string::npos)
        {
            line.append(";");
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

    if (HAD_ERROR)
    {
        exit(65);
    }
    if (HAD_RUNTIME_ERROR)
    {
        exit(75);
    }
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

void Lox::error(Token* token, std::string message)
{
    if (token->getType() == TokenType::__EOF__)
    {
        report(token->getLine(), " at end", message);
        free(token);
    }
    else
    {
        report(token->getLine(), " at '" + token->getLexeme() + "'", message);
        free(token);
    }
}

void Lox::runtimeError(RuntimeError error)
{
    std::cout << error.getMessage() + "\n[line " + std::to_string(error.getToken()->getLine()) +
        "]";
}
