#pragma once

#include <token_type.hpp>
#include <string>
#include <variant>

using Literal = std::variant<double, std::string>;

class Token
{
  public:
    explicit Token(TokenType type, std::string lexeme, Literal literal, int line);

    std::string toString();

    static std::string displayType(const TokenType token);
    static std::string displayLiteral(const TokenType type, const Literal literal);

  private:
    const Literal literal;
    const TokenType type;
    const std::string lexeme;
    const int line;
};
