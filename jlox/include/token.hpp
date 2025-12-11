#pragma once

#include <token_type.hpp>
#include <string>
#include <variant>

using LiteralValue = std::variant<double, std::string>;

class Token
{
  public:
    explicit Token(TokenType type, std::string lexeme, LiteralValue literal, int line);

    std::string toString() const;
    std::string getLexeme() const;

    static std::string displayType(const TokenType token);
    static std::string displayLiteral(const TokenType type, const LiteralValue literal);

  private:
    const LiteralValue literal;
    const TokenType type;
    const int line;
    const std::string lexeme;
};
