#pragma once

#include <exception>
#include <token.hpp>
#include <expr.hpp>

#include <vector>

class ParseError : public std::exception
{
};

class Parser
{
  public:
    Parser(std::vector<Token*> tokens)
      : tokens{ std::move(tokens) }
    {
    }

    Expr* parse();

  private:
    int current = 0;
    std::vector<Token*> tokens;

    Expr* expression();
    Expr* equality();
    Expr* comparison();
    Expr* term();
    Expr* factor();
    Expr* unary();
    Expr* primary();

    template<typename... T>
    bool match(T... types)
    {
        for (auto type : { types... })
        {
            if (check(type))
            {
                advance();
                return true;
            }
        }

        return false;
    }

    bool check(TokenType type);
    bool isAtEnd();
    Token* peek();
    Token* previous();
    Token* advance();
    Token* consume(TokenType type, std::string message);

    ParseError error(Token* token, std::string message);
    void synchronize();
};
