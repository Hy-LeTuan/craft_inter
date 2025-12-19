#pragma once

#include <token_type.hpp>
#include <string>
#include <literal_value.hpp>

class Token
{
  public:
    explicit Token(TokenType type, std::string lexeme, LiteralValue literal, int line);
    ~Token() = default;

    std::string toString() const;
    std::string getLexeme() const;
    TokenType getType() const;
    LiteralValue* getLiteral() const;
    std::string getLiteralAsString() const;
    int getLine() const;

    static std::string displayType(const TokenType token);
    static std::string displayLiteral(LiteralValue literal);

  private:
    const LiteralValue literal;
    const TokenType type;
    const int line;
    const std::string lexeme;
};
