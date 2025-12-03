#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>

bool HAD_ERROR = false;

void run(std::string source)
{
    if (HAD_ERROR)
    {
        exit(65);
    }

    std::istringstream sstream(source);

    char del = ' ';
    std::string line;
    std::string token;

    while (getline(sstream, line))
    {
        std::istringstream line_stream(line);

        while (getline(line_stream, token, del))
        {
            std::cout << "token: " << token << std::endl;
        }
    }
}

void runPrompt()
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

void runFile(const char* filename_ptr)
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

void report(int line, std::string where, std::string message)
{
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    HAD_ERROR = true;
}

void error(int line, std::string message)
{
    report(line, "", message);
}

int main(const int argc, const char** argv)
{
    if (argc > 2)
    {
        std::cout << "Usage: jlox [script]" << std::endl;
        return 64;
    }
    if (argc == 2)
    {
        runFile(argv[1]);
    }
    else if (argc == 1)
    {
        runPrompt();
    }

    return 0;
}
