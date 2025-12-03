#pragma once

#include <token_type.hpp>
#include <string>

std::string printTokenType(TokenType type);

class Token
{
  public:
    Token(TokenType type, std::string lexeme, void* literal, int line)
      : type{ type }
      , lexeme{ lexeme }
      , literal{ literal }
      , line{ line }
    {
    }

    std::string toString()
    {
        std::string name = printTokenType(type) + " " + lexeme + " " + "[insert literal here]";
        return name;
    }

  private:
    const TokenType type;
    const std::string lexeme;
    const void* literal;
    const int line;
};
