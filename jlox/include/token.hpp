#pragma once

#include <token_type.hpp>
#include <string>
#include <object.hpp>

class Token
{
  public:
    explicit Token(TokenType type, std::string lexeme, Object literal, int line);

    std::string toString() const;
    std::string getLexeme() const;
    TokenType getType() const;
    Object* getLiteral() const;
    std::string getLiteralAsString() const;
    int getLine() const;

    static std::string displayType(const TokenType token);
    static std::string displayLiteral(Object literal);

  private:
    const Object literal;
    const TokenType type;
    const int line;
    const std::string lexeme;
};
