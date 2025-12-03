#pragma once
#include "token_type.hpp"
#include <string>
#include <token.hpp>
#include <vector>

class Scanner
{
  public:
    Scanner(std::string source)
      : source{ source }
    {
    }

    std::vector<Token> scanTokens()
    {
        while (!isAtEnd())
        {
            start = current;
            scanToken();
        }

        tokens.push_back(Token(TokenType::__EOF__, "", nullptr, line));
        return tokens;
    }

  private:
    const std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd()
    {
        return current >= source.length();
    }

    void scanToken()
    {
        char c = advance();

        switch (c)
        {
            case '(':
                addToken(TokenType::LEFT_PAREN);
                break;
            case ')':
                addToken(TokenType::RIGHT_PAREN);
                break;
            case '{':
                addToken(TokenType::LEFT_BRACE);
                break;
            case '}':
                addToken(TokenType::RIGHT_BRACE);
                break;
            case ',':
                addToken(TokenType::COMMA);
                break;
            case '.':
                addToken(TokenType::DOT);
                break;
            case '-':
                addToken(TokenType::MINUS);
                break;
            case '+':
                addToken(TokenType::PLUS);
                break;
            case ';':
                addToken(TokenType::SEMICOLON);
                break;
            case '*':
                addToken(TokenType::STAR);
                break;
        }
    }

    char advance()
    {
        return source.at(current++);
    }

    void addToken(TokenType type)
    {
        addToken(type, nullptr);
    }

    void addToken(TokenType type, void* literal)
    {
        std::string text = source.substr(start, current);
        tokens.push_back(Token(type, text, literal, line));
    }
};
